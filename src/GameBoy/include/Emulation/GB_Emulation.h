#ifndef GB_EMULATION_H
#define GB_EMULATION_H

#include <Emulation/GB_Instruction.h>
#include <stdint.h>
#include <stdlib.h>
#include <Memory/GB_Header.h>

#define GB_INVALID_INSTRUCTION 0xAEAE
// CPU FREQ  4.194304 hz or 238.41857910156 ns
#define GB_DMG_CPU_FREQ_NS 238.41f

uint8_t GB_Initialize(int argc, const char ** argv);
long    GB_LoadProgram(const char *filePath);
void    GB_QuitProgram();
void    GB_TickTimers();
int     GB_TickEmulation();
void    GB_SetEmulationContext(const void *context);
// INTERNAL
void    GB_PopulateMemory(const uint8_t *buffer, size_t bytesRead);
GameBoyInstruction* GB_FetchInstruction(const uint8_t opcode);
void    GB_ParseRom(const uint8_t *buffer, size_t size);
void    GB_PrintRomInfo(const GB_Header * header);

#endif