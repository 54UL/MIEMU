#ifndef GB_INSTRUCTIONS_H
#define GB_INSTRUCTIONS_H
#include <Emulation/GB_SystemContext.h>

#define GB_INSTRUCTION_SET_LENGHT 0xFF

// Condition types (enumarated)
#define COND_NZ 0
#define COND_Z  1
#define COND_NC 2
#define COND_C  3

// 8-BIT LOAD INSTRUCTIONS
uint8_t GB_LD_R_R(EmulationState *ctx);
uint8_t GB_LD_R_N(EmulationState *ctx);
uint8_t GB_LD_R_HL(EmulationState *ctx);
uint8_t GB_LD_HL_R(EmulationState *ctx);
uint8_t GB_LD_HL_N(EmulationState *ctx);
uint8_t GB_LD_A_BC(EmulationState *ctx);
uint8_t GB_LD_A_DE(EmulationState *ctx);
uint8_t GB_LD_A_NN(EmulationState *ctx);
uint8_t GB_LD_BC_A(EmulationState *ctx);// NOT USED??
uint8_t GB_LD_DE_A(EmulationState *ctx);// NOT USED??
uint8_t GB_LD_NN_A(EmulationState *ctx);
uint8_t GB_LDH_A_N(EmulationState *ctx);
uint8_t GB_LDH_N_A(EmulationState *ctx);
uint8_t GB_LDH_A_C(EmulationState *ctx);
uint8_t GB_LDH_C_A(EmulationState *ctx);
uint8_t GB_LDI_HL_A(EmulationState *ctx);
uint8_t GB_LDI_A_HL(EmulationState *ctx);
uint8_t GB_LDD_HL_A(EmulationState *ctx);
uint8_t GB_LDD_A_HL(EmulationState *ctx);

// 16 BIT LOAD INSTRUCTIONS
uint8_t GB_LD_RR_NN(EmulationState *ctx);
uint8_t GB_LD_NN_SP(EmulationState *ctx);
uint8_t GB_LD_SP_HL(EmulationState *ctx);
uint8_t GB_PUSH_RR(EmulationState *ctx);
uint8_t GB_POP_RR(EmulationState *ctx);

// 8 BIT ALU INSTRUCTIONS
uint8_t GB_ADD_A_R(EmulationState *ctx);
uint8_t GB_ADD_A_N(EmulationState *ctx);
uint8_t GB_ADD_A_HL(EmulationState *ctx);
uint8_t GB_ADC_A_R(EmulationState *ctx);
uint8_t GB_ADC_A_N(EmulationState *ctx);
uint8_t GB_ADC_A_HL(EmulationState *ctx);
uint8_t GB_SUB_R(EmulationState *ctx);
uint8_t GB_SUB_N(EmulationState *ctx);
uint8_t GB_SUB_HL(EmulationState *ctx);
uint8_t GB_SBC_A_R(EmulationState *ctx);
uint8_t GB_SBC_A_N(EmulationState *ctx);
uint8_t GB_SBC_A_HL(EmulationState *ctx);
uint8_t GB_AND_R(EmulationState *ctx);
uint8_t GB_AND_N(EmulationState *ctx);
uint8_t GB_AND_HL(EmulationState *ctx);
uint8_t GB_XOR_R(EmulationState *ctx);
uint8_t GB_XOR_N(EmulationState *ctx);
uint8_t GB_XOR_HL(EmulationState *ctx);
uint8_t GB_OR_R(EmulationState *ctx);
uint8_t GB_OR_N(EmulationState *ctx);
uint8_t GB_OR_HL(EmulationState *ctx);
uint8_t GB_CP_R(EmulationState *ctx);
uint8_t GB_CP_N(EmulationState *ctx);
uint8_t GB_CP_HL(EmulationState *ctx);
uint8_t GB_INC_R(EmulationState *ctx);
uint8_t GB_INC_HL(EmulationState *ctx);
uint8_t GB_DEC_R(EmulationState *ctx);
uint8_t GB_DEC_HL(EmulationState *ctx);
uint8_t GB_DAA(EmulationState *ctx);
uint8_t GB_CPL(EmulationState *ctx);

// 16 BIT ALU INSTRUCTIONS
uint8_t GB_ADD_HL_RR(EmulationState *ctx);
uint8_t GB_INC_RR(EmulationState *ctx);
uint8_t GB_DEC_RR(EmulationState *ctx);
uint8_t GB_ADD_SP_DD(EmulationState *ctx);
uint8_t GB_LD_HL_SP_PLUS_DD(EmulationState *ctx);

// ROTATE AND SHIFT INSTRUCTIONS
uint8_t GB_RLCA(EmulationState *ctx);
uint8_t GB_RLA(EmulationState *ctx);
uint8_t GB_RRCA(EmulationState *ctx);
uint8_t GB_RRA(EmulationState *ctx);
uint8_t GB_RLC_R(EmulationState *ctx);
uint8_t GB_RLC_HL(EmulationState *ctx);
uint8_t GB_RL_R(EmulationState *ctx);
uint8_t GB_RL_HL(EmulationState *ctx);
uint8_t GB_RRC_R(EmulationState *ctx);
uint8_t GB_RRC_HL(EmulationState *ctx);
uint8_t GB_RR_R(EmulationState *ctx);
uint8_t GB_RR_HL(EmulationState *ctx);
uint8_t GB_SLA_R(EmulationState *ctx);
uint8_t GB_SLA_HL(EmulationState *ctx);
uint8_t GB_SWAP_R(EmulationState *ctx);
uint8_t GB_SWAP_HL(EmulationState *ctx);
uint8_t GB_SRA_R(EmulationState *ctx);
uint8_t GB_SRA_HL(EmulationState *ctx);
uint8_t GB_SRL_R(EmulationState *ctx);
uint8_t GB_SRL_HL(EmulationState *ctx);

// CPU CONTROL INSTRUCTIONS
uint8_t GB_CCF(EmulationState *ctx);
uint8_t GB_SCF(EmulationState *ctx);
uint8_t GB_NOP(EmulationState *ctx);
uint8_t GB_HALT(EmulationState *ctx);
uint8_t GB_STOP(EmulationState *ctx);
uint8_t GB_DI(EmulationState *ctx);
uint8_t GB_EI(EmulationState *ctx);

// JUMP INSTRUCTIONS
uint8_t GB_JP_NN(EmulationState *ctx);
uint8_t GB_JP_HL(EmulationState *ctx);
uint8_t GB_JP_CC_NN(EmulationState *ctx);
uint8_t GB_JR_E(EmulationState *ctx);
uint8_t GB_JR_CC_E(EmulationState *ctx);

uint8_t GB_CALL_NN(EmulationState *ctx);
uint8_t GB_CALL_CC_NN(EmulationState *ctx);

uint8_t GB_RET(EmulationState *ctx);
uint8_t GB_RET_CC(EmulationState *ctx);
uint8_t GB_RETI(EmulationState *ctx);

uint8_t GB_RST_N(EmulationState *ctx);


// SINGLE BIT OPERATIONS (CB PREFIX)
uint8_t GB_CB_BIT_N_R(EmulationState *ctx);   
uint8_t GB_CB_BIT_N_HL(EmulationState *ctx);
uint8_t GB_CB_SET_N_R(EmulationState *ctx);   
uint8_t GB_CB_SET_N_HL(EmulationState *ctx);
uint8_t GB_CB_RES_N_R(EmulationState *ctx);   
uint8_t GB_CB_RES_N_HL(EmulationState *ctx);

uint8_t GB_CB_PREFIX_INSTRUCTIONS(EmulationState *ctx);

// AXULIAR DECODING FUNCTIONS
uint8_t GB_ResolveCondition(const EmulationState *ctx, uint8_t cc);

#endif