#ifndef EMULATOR_SHELL_H
#define EMULATOR_SHELL_H

#include <stdint.h>
#include <minemu.h>

#define DEFAUL_COL_Y_SPACING 8
#define CC8_ROMS_PATH "../roms/"

void       EmuShell_Init();
void       EmuShell_UpdateFrame(uint32_t * pixels);
void       EmuShell_KeyPressed(const char code);
uint8_t    EmuShell_Shown();
void       EmuShell_ShellAction(const ShellAction action, ShellCallback callback);
void       EmuShell_SetState(const ShellState state);
ShellState EmuShell_GetState();

#endif
