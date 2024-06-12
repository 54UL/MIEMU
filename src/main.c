// MINEMU APP (UI + CORE + STATIC LINKED EMULATORS)
#include <App.h>
#include <minemu.h>
#include <CC8_Chip8.h>

// Api elements
EmuApp *app;
Emulation *emulator;

// App callbacks
void OnRender(unsigned int *pixels);

// UI Shell callbacks
void StartEmulation(void * data);
void StopEmulation(void * data);
void QuitEmulation(void * data);

int main(int argc, char **argv)
{
    uint8_t  running = 1 ;
    uint32_t last_update_time;
    uint32_t last_update_time_timers;

    //TODO: ADD APP SELECTOR
    app = &TinySDLApp;
    //TODO: ADD EMULATOR SELECTOR
    emulator = &Chip8Emulator;

    // Fetch default emulation config
    EmulationInfo emuInfo = emulator->GetInfo();

    // Configure emulator shell actions (if available)
    EmulatorUI.ShellAction(Start, StartEmulation);
    EmulatorUI.ShellAction(Stop, StopEmulation);
    EmulatorUI.ShellAction(Quit, QuitEmulation);

    // App and emulator initialization
    app->Init(&emuInfo, emulator->OnInput, &EmulatorUI);

    // Main loop
    while (running)
    {
        uint32_t frameBeginTicks = app->GetTicks();
        uint32_t delta_time = frameBeginTicks - last_update_time;

        // TODO: WHY TF DOES THE RENDER STATUS CONTROL THE MAIN LOOP???
        running = app->Render(OnRender);

        if (EmulatorUI.GetState() == Running)
        {
            emulator->Loop(last_update_time, delta_time);
        }

        last_update_time = frameBeginTicks;
    }

    // App termination
    emulator->QuitProgram();
    app->Exit();
}

void OnRender(unsigned int *pixels)
{
    // TODO: ADD REAL TIME WINDOW HEIGHT/WIDTH
    emulator->OnRender(pixels, 0,0);
}

void StartEmulation(void * data)
{
    const ShellState sate = EmulatorUI.GetState();
    switch (sate)
    {
        case Running:
            EmulatorUI.SetState(Starting);
            emulator->QuitProgram();
            break;

        default:
            break;
    }
    
    if (emulator->LoadProgram((const char*) data) > 1)
    {
        EmulatorUI.SetState(Running);
    }
    else
    {
        EmulatorUI.SetState(Exception);
    }
}

void StopEmulation(void * data)
{
    MNE_Log("StopEmulation\n");
}

void QuitEmulation(void * data)
{
    MNE_Log("QuitEmulation\n");
}