#include "SDL_TinyApp.h"
#include "../UI/EmulatorShell.h"
#include <SDL2/SDL.h>

#define SCREEN_FACTOR 8

// Window/presentation
static SDL_Window *s_window;
static SDL_Renderer *s_renderer;

// Emulator shell
static EmulatorShell * s_emulator_shell;

// Events
SDL_Thread* eventThread;
static ActionCallback s_eventCallback;
static SDL_Event * event;
static int quitStatus = 1 ;

// Rendering
static uint32_t s_last_update_time;
static unsigned int *s_emulation_pixels;
static unsigned int *s_emulator_ui_pixels;

static SDL_Texture *s_screen_texture;
static SDL_Texture *s_emulator_ui_texture;

static uint64_t s_rendering_ticks;
static int s_emulation_frame_width, s_emulation_frame_height;
static int s_emulator_ui_frame_width, s_emulator_ui_frame_height;

char DesktopKeyMapping(const char code)
{
    switch (code)
    {
    // 1 2 3 C
    case SDLK_1:
        return 0x01;
    case SDLK_2:
        return 0x02;
    case SDLK_3:
        return 0x03;
    case SDLK_4:
        return 0x0C;
    // 4 5 6 D
    case SDLK_q:
        return 0x04;
    case SDLK_w:
        return 0x05;
    case SDLK_e:
        return 0x06;
    case SDLK_r:
        return 0x0D;
    // 7 8 9 E
    case SDLK_a:
        return 0x07;
    case SDLK_s:
        return 0x08;
    case SDLK_d:
        return 0x09;
    case SDLK_f:
        return 0x0E;
    // A 0 B F
    case SDLK_z:
        return 0x0A;
    case SDLK_x:
        return 0x00;
    case SDLK_c:
        return 0x0B;
    case SDLK_v:
        return 0x0F;
    case -100:
        return 0xFF;
    }
}

int EventThreadFunction()
{
    SDL_Event event;
   
    while (quitStatus)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    quitStatus = 0;
                    return 0;

                case SDL_KEYDOWN:
                    s_emulator_shell->OnInput(event.key.keysym.sym);
                    s_eventCallback(DesktopKeyMapping(event.key.keysym.sym));
                    break;

                case SDL_KEYUP:
                    s_eventCallback(-100);
                    s_emulator_shell->OnInput(-100);
                    break;

                default:
                    break;
            }
        }
    }

    return 1;
}

void Init_EventThread()
{
    // Create a thread for event handling
    eventThread = SDL_CreateThread(EventThreadFunction, "EventThread", s_eventCallback);

    if (eventThread == NULL)
    {
        MNE_Log("Cannot create event thread....\n");
    }
}



void audioCallback(void* userdata, Uint8* stream, int len) {
    // static int frequency = 440; // Adjust this for different frequencies
    static const int AMPLITUDE = 28000;
    static const int SAMPLE_RATE = 44100;
    // BASIC SQUARE GENERATION
    static int phase = 0;
    int frequency = *(int*)(userdata);

    for (int i = 0; i < len; i += 2) {
        int sample = (phase++ % (SAMPLE_RATE / frequency)) < (SAMPLE_RATE / (2 * frequency)) ? AMPLITUDE : -AMPLITUDE;
        stream[i] = (uint8_t)(sample & 0xFF);
        stream[i + 1] = (uint8_t)((sample >> 8) & 0xFF);
    }
}

static SDL_AudioSpec s_audioSpec;

void Init_App_Audio()
{
    if (SDL_Init(SDL_INIT_AUDIO) != 0) 
    {
        MNE_Log("Failed to open audio: %s\n", SDL_GetError());
        return;
    }

    s_audioSpec.freq = 44100;
    s_audioSpec.format = AUDIO_S16SYS;
    s_audioSpec.channels = 1;
    s_audioSpec.samples = 1024;
    s_audioSpec.callback = audioCallback;
    s_audioSpec.userdata = (void *) 440; // Police siren freq (ignore the cast lol)
}

void PlaySquareWave(int frequency, int duration) 
{
    SDL_AudioSpec obtainedSpec;
    if (SDL_OpenAudio(&s_audioSpec, &obtainedSpec) != 0) 
    {
        MNE_Log("Failed to open audio: %s\n", SDL_GetError());
        return;
    }
    SDL_PauseAudio(0);

    // Play the beep for the specified duration
    SDL_Delay(duration);

    SDL_CloseAudio();
}

// END AUDIO IMPLEMENTATION

void Init_App(EmulationInfo *info, ActionCallback actionsCallback, EmulatorShell * shell)
{
    // App initialization...
    quitStatus = 1;
    s_emulator_shell = NULL;
    s_eventCallback = actionsCallback;

    if (s_emulator_shell != shell)
    {
        s_emulator_shell = shell;
    }

    // SDL audio initialization
    Init_App_Audio();
    
    // SDL Video initialization
    SDL_Init(SDL_INIT_VIDEO);

    s_window = SDL_CreateWindow(info->name,
                                SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                info->displayWidth, info->displayHeight,
                                SDL_WINDOW_RESIZABLE);

    s_renderer = SDL_CreateRenderer(s_window,
                                    -1, SDL_RENDERER_ACCELERATED);

    SDL_SetRenderTarget(s_renderer, NULL);  // Set the render target to the default

    s_emulation_frame_width = info->displayWidth;
    s_emulation_frame_height = info->displayHeight;

    //TESTING EMULATOR BUFFER SIZE: 300 X 128 
    s_emulator_ui_frame_width = info->UIConfig.frameWidth;
    s_emulator_ui_frame_height = info->UIConfig.frameHeight;

    // Since we are going to display a low resolution buffer
    // it is best to limit the window size so that it cannot
    // be smaller than our internal buffer size.
    SDL_SetWindowMinimumSize(s_window, s_emulation_frame_width * SCREEN_FACTOR, s_emulation_frame_height * SCREEN_FACTOR);
    SDL_RenderSetLogicalSize(s_renderer, s_emulation_frame_width * SCREEN_FACTOR, s_emulation_frame_height * SCREEN_FACTOR);
    SDL_RenderSetIntegerScale(s_renderer, 1);

    s_screen_texture = SDL_CreateTexture(s_renderer,
                                         SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
                                         s_emulation_frame_width, s_emulation_frame_height);

    s_emulator_ui_texture = SDL_CreateTexture(s_renderer,
                                         SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
                                         s_emulator_ui_frame_width , s_emulator_ui_frame_height );

    MNE_New(s_emulation_pixels, s_emulation_frame_width * s_emulation_frame_height, int);
    MNE_New(s_emulator_ui_pixels, s_emulator_ui_frame_height * s_emulator_ui_frame_width, int);

    // Randomize the buffer (ready state) (Emulation RENDERING TEST)
    int i = 0,j = 0;
    for (i = 0; i < s_emulation_frame_height; i++) 
    {
        for (j = 0; j < s_emulation_frame_width; j++)
        {
             s_emulation_pixels[i+j*s_emulation_frame_height] = rand() % 0xFFFFFF7F;
        }
    }

    // Emulator buffer test.
    for (i = 0; i < s_emulator_ui_frame_height; i++) 
    {
        for (j = 0; j < s_emulator_ui_frame_width; j++)
        {
            s_emulator_ui_pixels[i+j*s_emulator_ui_frame_height] = 0x1E1E1E1E;
        }
    }

    // Init event thread...
    Init_EventThread((void *) actionsCallback);

    // Init emulator shell (IF AVAILABLE)
    if (s_emulator_shell != NULL)
    {
        s_emulator_shell->Init();
    }
}

void Render()
{
    // Clear the renderer
    SDL_SetRenderTarget(s_renderer, NULL);

    SDL_SetRenderDrawColor(s_renderer, 0, 0, 0, 0);
    SDL_RenderClear(s_renderer);

    // SDL_SetRenderDrawBlendMode(s_renderer, SDL_BLENDMODE_BLEND);

    // Emulation display rendering
    SDL_UpdateTexture(s_screen_texture, NULL, s_emulation_pixels, s_emulation_frame_width * 4);
    SDL_RenderCopy(s_renderer, s_screen_texture, NULL, NULL);

    // Emulator display rendering
    SDL_UpdateTexture(s_emulator_ui_texture, NULL, s_emulator_ui_pixels, s_emulator_ui_frame_width * 4);

    // Emulation rendering buffer area
    SDL_Rect destinationRect;
    destinationRect.x = 0;
    destinationRect.y = 0;
    destinationRect.w = s_emulator_ui_frame_width ;
    destinationRect.h = s_emulator_ui_frame_height;

    // Emulator frame buffer area
    SDL_Rect sourceRect;
    sourceRect.x = 0;
    sourceRect.y = 0;
    sourceRect.w = s_emulator_ui_frame_width ;
    sourceRect.h = s_emulator_ui_frame_width ;

    if (s_emulator_shell->Shown())
    {
        SDL_RenderCopyEx(s_renderer, s_emulator_ui_texture, &sourceRect, &destinationRect, 0, NULL, SDL_FLIP_NONE);
    }

    // Update the screen
    SDL_RenderPresent(s_renderer);
}

uint8_t Step_SDL(StepCallback renderCallback)
{
    // Toy Rendering pipeline (monothread)
    // UI RENDER UPDATES -> EMU RENDER UPDATES -> DRAWCALL(Render)
    s_emulator_shell->UpdateFrame(s_emulator_ui_pixels);
    renderCallback(s_emulation_pixels);
    Render();
    return quitStatus;
}

uint32_t GetTicks_SDL()
{
    return SDL_GetTicks();
}

void Reset_SDL(void)
{
    free(s_emulation_pixels);
    free(s_emulator_ui_pixels);
}

void Exit_SDL_App(void)
{
    int eventThreadReturnValue;
    SDL_WaitThread(eventThread, &eventThreadReturnValue);
    MNE_Log("Event thread returned:%i\n",eventThreadReturnValue);

    free(s_emulation_pixels);
    free(s_emulator_ui_pixels);
    
    // Release SDL
    SDL_CloseAudio();
    SDL_Quit();
}

