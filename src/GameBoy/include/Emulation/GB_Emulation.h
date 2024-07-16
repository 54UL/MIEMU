#ifndef GB_EMULATION_H
#define GB_EMULATION_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <minemu.h>
#include <Emulation/GB_Instruction.h>
#include <Memory/GB_Header.h>
#include <SOC/GB_LCD.h>
#include <SOC/GB_Bus.h>
#include <SOC/GB_CPU.h>
#include <SOC/GB_Opcodes.h>

//0XD3 IS THE FIRST NOT VALID DMG OPCODE 
#define GB_INVALID_INSTRUCTION 0xD3

// CPU FREQ  4.194304 hz or 238.41857910156 ns
#define GB_DMG_CPU_FREQ_NS 238.41f

EmulationInfo GB_GetInfo();
uint8_t       GB_Initialize(int argc, const char ** argv);
long          GB_LoadProgram(const char *filePath);
void          GB_QuitProgram();
void          GB_TickTimers();
int           GB_TickEmulation();
void          GB_SetEmulationContext(const void *context);
void          GB_OnRender(uint32_t* pixels, const int64_t w, const int64_t h);

// INTERNAL
uint8_t             GB_TickCpu();
uint8_t             GB_HandleInterrupts();

void                GB_PopulateMemory(const uint8_t *buffer, size_t bytesRead);
GameBoyInstruction* GB_FetchInstruction(const uint8_t opcode);
void                GB_ParseRom(const uint8_t *buffer, size_t size);
void                GB_PrintRomInfo(const GB_Header * header);

#endif