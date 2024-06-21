#ifndef GB_INSTRUCTION_H
#define GB_INSTRUCTION_H
#include <Emulation/GB_SystemContext.h>

typedef uint8_t (*instructionFnPtrGb)(EmulationState *ctx);

typedef struct {
    uint16_t maskl;
    // uint16_t maskh;
    uint16_t opcode;
    instructionFnPtrGb handler;
#ifdef GB_DEBUG
    const char * handlerName;
#endif

} GameBoyInstruction;

#ifdef GB_DEBUG
#define GB_INSTRUCTION(mask, opcode, handler)  {mask, opcode, (instructionFnPtrGb)handler, ""#handler" [MASK:"#mask"] [CODE:"#opcode"] [CURRENT:%04X] [PC:%04X]\n"}
#else
#define GB_INSTRUCTION(mask, opcode, handler)  {mask, opcode, (instructionFnPtrGb)handler}
#endif

#endif