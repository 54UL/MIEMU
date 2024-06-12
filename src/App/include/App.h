#ifndef APP_H
#define APP_H
#include <minemu.h>

#include "../UI/EmulatorShell.h"
#include "../SDL_TinyAppImpl/SDL_TinyApp.h"

//App implementation
EmuApp TinySDLApp = {
    .Init   = Init_App,
    .Render = Step_SDL,
    .GetTicks = GetTicks_SDL,
    .Reset  = Reset_SDL,
    .Exit   = Exit_SDL_App
};

EmulatorShell EmulatorUI = 
{
    .Init        = EmuShell_Init,
    .UpdateFrame = EmuShell_UpdateFrame,
    .Shown       = EmuShell_Shown,
    .OnInput     = EmuShell_KeyPressed,
    .ShellAction = EmuShell_ShellAction,
    .SetState    = EmuShell_SetState,
    .GetState    = EmuShell_GetState
};


#endif