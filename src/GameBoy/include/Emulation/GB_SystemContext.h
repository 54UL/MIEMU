#ifndef GB_SYSTEM_CONTEXT_H
#define GB_SYSTEM_CONTEXT_H

#include <stdint.h>
#include <CPU/GB_Registers.h>
#include <Memory/GB_Header.h>

#define GB_MEMORY_SIZE 0xFFFF

typedef struct
{
    // CPU STATE
    uint8_t         IE;
    uint8_t         HALT;
    
    uint8_t         *memory;
    GB_Registers    *registers;
    // Cartige
    GB_Header       *header;
} EmulationState;

typedef uint8_t (*instructionFnPtrGb)(EmulationState *ctx);

#endif