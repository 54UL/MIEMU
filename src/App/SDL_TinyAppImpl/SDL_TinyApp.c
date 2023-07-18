#include "SDL_TinyApp.h"
#include "../UI/EmulatorShell.h"

#include <SDL2/SDL.h>


// Window/presentation
static SDL_Window *s_window;
static SDL_Renderer *s_renderer;
// Emulator shell
static EmulatorShell * s_emulator_shell;

// Events
SDL_Thread* eventThread;
static ActionCallback s_eventCallback;
static SDL_Event * event;
static volatile quitStatus = 1 ;

// Rendering
static uint32_t s_last_update_time;
static unsigned int *s_chip8_pixels;
static unsigned int *s_emulator_pixels;

static SDL_Texture *s_screen_texture;
static SDL_Texture *s_emulator_ui_texture;

static uint64_t s_rendering_ticks;
static int s_chip8_frame_width, s_chip8_frame_height;
static int s_emulator_frame_width, s_emulator_frame_height;
#define SCREEN_FACTOR 8

int EventThreadFunction()
{
    SDL_Event event;
   
    while (quitStatus)
    {
        /* code */
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    quitStatus = 0;
                    return 0;

                case SDL_KEYDOWN:
                    s_emulator_shell->OnInput(event.key.keysym.sym);
                    s_eventCallback(event.key.keysym.sym);
                    break;

                case SDL_KEYUP:
                    s_eventCallback(-100);
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
        // std::cerr << "Thread creation failed: " << SDL_GetError() << std::endl;
        printf("Cannot create event thread....\n");
        // return 1;
    }
}


void Init_App(uint16_t w, uint16_t h, ActionCallback actionsCallback, EmulatorShell * shell)
{
    // CC8 App initialization...
    quitStatus = 1;
    s_emulator_shell = NULL;
    s_eventCallback = actionsCallback;

    if (s_emulator_shell != shell)
    {
        s_emulator_shell = shell;
    }

    // SDL Initialization
    SDL_Init(SDL_INIT_VIDEO);

    s_window = SDL_CreateWindow("CC8",
                                SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                w, h,
                                SDL_WINDOW_RESIZABLE);

    s_renderer = SDL_CreateRenderer(s_window,
                                    -1, SDL_RENDERER_ACCELERATED);

    SDL_SetRenderTarget(s_renderer, NULL);  // Set the render target to the default

    s_chip8_frame_width = w;
    s_chip8_frame_height = h;

    //TESTING EMULATOR BUFFER SIZE: 300 X 128 
    s_emulator_frame_width = 300;
    s_emulator_frame_height = 128;

    // Since we are going to display a low resolution buffer
    // it is best to limit the window size so that it cannot
    // be smaller than our internal buffer size.
    SDL_SetWindowMinimumSize(s_window, s_chip8_frame_width * SCREEN_FACTOR, s_chip8_frame_height * SCREEN_FACTOR);
    SDL_RenderSetLogicalSize(s_renderer, s_chip8_frame_width * SCREEN_FACTOR, s_chip8_frame_height * SCREEN_FACTOR);
    SDL_RenderSetIntegerScale(s_renderer, 1);

    s_screen_texture = SDL_CreateTexture(s_renderer,
                                         SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
                                         s_chip8_frame_width, s_chip8_frame_height);

    s_emulator_ui_texture = SDL_CreateTexture(s_renderer,
                                         SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
                                         s_emulator_frame_width , s_emulator_frame_height );

    s_chip8_pixels = calloc(s_chip8_frame_width * s_chip8_frame_height, sizeof(int));
    s_emulator_pixels = calloc(s_emulator_frame_height * s_emulator_frame_width, sizeof(int));

    //Randomize the buffer (ready state) (CHIP 8 RENDERING TEST)
    int i = 0,j = 0;
    for (i = 0; i < s_chip8_frame_height; i++) 
    {
        for (j = 0; j < s_chip8_frame_width; j++)
        {
             s_chip8_pixels[i+j*s_chip8_frame_height] = rand() % 0xFFFFFF7F;
        }
    }

    // Emulator buffer test.
    for (i = 0; i < s_emulator_frame_height; i++) 
    {
        for (j = 0; j < s_emulator_frame_width; j++)
        {
            s_emulator_pixels[i+j*s_emulator_frame_height] = 0x1E1E1E1E;
        }
    }

    //Init event thread...
    Init_EventThread((void *) actionsCallback);
    s_emulator_shell->Init();
}

void Render()
{
    // Clear the renderer
    SDL_SetRenderTarget(s_renderer, NULL);

    SDL_SetRenderDrawColor(s_renderer, 0, 0, 0, 0);
    SDL_RenderClear(s_renderer);

    // // Set the blend mode to enable texture blending
    // SDL_SetRenderDrawBlendMode(s_renderer, SDL_BLENDMODE_BLEND);

    // Chip 8 display rendering
    SDL_UpdateTexture(s_screen_texture, NULL, s_chip8_pixels, s_chip8_frame_width * 4);
    SDL_RenderCopy(s_renderer, s_screen_texture, NULL, NULL);

    // Emulator display rendering
    SDL_UpdateTexture(s_emulator_ui_texture, NULL, s_emulator_pixels, s_emulator_frame_width * 4);

    // Chip 8 rendering buffer area
    SDL_Rect destinationRect;
    destinationRect.x = 0;
    destinationRect.y = 0;
    destinationRect.w = s_emulator_frame_width ;
    destinationRect.h = s_emulator_frame_height;

    // Emulator frame buffer area
    SDL_Rect sourceRect;
    sourceRect.x = 0;
    sourceRect.y = 0;
    sourceRect.w = s_emulator_frame_width ;
    sourceRect.h = s_emulator_frame_width ;

    if (s_emulator_shell->Shown())
        SDL_RenderCopyEx(s_renderer, s_emulator_ui_texture, &sourceRect, &destinationRect, 0, NULL, SDL_FLIP_NONE);

    // Update the screen
    SDL_RenderPresent(s_renderer);
}

uint8_t Step_SDL(StepCallBack renderCallback)
{
    Uint32  current_time = SDL_GetTicks();
    Uint32  delta_time = current_time - s_last_update_time;

    //TODO: FIX TIMING ISSUES
    if (delta_time >= 16)
    {
        // Call callbacks 
        s_emulator_shell->UpdateFrame(s_emulator_pixels);
        renderCallback(s_chip8_pixels);
        Render();

        //Update frame time...
        s_last_update_time = current_time;
    }   

    
    return quitStatus;
}

void Reset_SDL(void)
{
    free(s_chip8_pixels);
    free(s_emulator_pixels);
}

void Exit_SDL_App(void)
{
    int eventThreadReturnValue;
    SDL_WaitThread(eventThread, &eventThreadReturnValue);
    printf("Event thread returned:%i\n",eventThreadReturnValue);

    free(s_chip8_pixels);
    free(s_emulator_pixels);
}

