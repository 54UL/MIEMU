#ifndef SDL_TINY_APP_H
#define SDL_TINY_APP_H

#include <stdint.h>
#include <minemu.h>

// Emu app api implementation...
void     Init_App(EmulationInfo *info, ActionCallback actionsCallback, EmulatorShell * shell);
uint8_t  Step_SDL(StepCallback renderCallback);
uint32_t GetTicks_SDL(void);
void     Reset_SDL(void);
void     Exit_SDL_App(void);
char     DesktopKeyMapping(const char code);

// Internal usage...
void Init_App_Audio();
void PlaySquareWave(int frequency, int duration);

#endif 