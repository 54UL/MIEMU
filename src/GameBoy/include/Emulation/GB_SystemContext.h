#ifndef GB_SYSTEM_CONTEXT_H
#define GB_SYSTEM_CONTEXT_H

#include <stdint.h>
#include <SOC/GB_Registers.h>
#include <Memory/GB_Header.h>

typedef struct
{
    //PPU
    uint16_t ppuCycles;
    uint8_t  ppuMode;

    // CPU
    uint16_t cpuCycles; 
    uint8_t  ime;
    uint8_t  bios_enabled;
 
    // TODO: MOVE THIS, MEMORY SHOULD BE ACCESED BY BUS READ AND BUS WRITE IF U WANT TO KNOW A SPECIFIC MEMORY REGION...
    uint8_t         *bios;
    uint8_t         *bank_00;
    uint8_t         *vram;
    uint8_t         *hram;
    

    GB_Registers    *registers;
    // Cartige
    GB_Header       *header;
} EmulationState;


#endif