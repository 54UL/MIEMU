#ifndef GB_INSTRUCTION_H
#define GB_INSTRUCTION_H
#include <Emulation/GB_SystemContext.h>

//TODO: DEFINE THIS SYMBOL ON CMAKE AND NOT HERE...
#define GB_DEBUG

typedef struct {
    uint16_t mask;
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