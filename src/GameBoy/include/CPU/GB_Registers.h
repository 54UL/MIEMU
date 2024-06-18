#ifndef GB_REGISTERS_H
#define GB_REGISTERS_H

#include <stdint.h>

// 16 bit registers instruction encoding offests
#define GB_BC_OFFSET 0
#define GB_DE_OFFSET 1
#define GB_HL_OFFSET 2
#define GB_AF_OFFSET 3
#define GB_SP_OFFSET 3

//REGISTERS OFFSETS
// LSB MSB ARE FLIPPED THAT'S WHY B = 1 AND C = 0 AND 
#define GB_B_OFFSET           1
#define GB_C_OFFSET           0
#define GB_D_OFFSET           3
#define GB_E_OFFSET           2
#define GB_H_OFFSET           5
#define GB_L_OFFSET           4
#define GB_HL_INDIRECT_OFFSET 6
#define GB_A_OFFSET           7

//FLAGS
#define GB_ZERO_FLAG    7
#define GB_N_FLAG       6
#define GB_H_FLAG       5
#define GB_C_FLAG       4

//TODO: WHEN WIDELY USED CHECK PERFORMANCE REPLACING FOR A FUNCTION...
//TODO: REFACTORIZE  AND REMOVE USELESS SHIT: like the tmpRegF fuckery (FOR FUCKING REAL)
#define GB_U8_TO_U16(LSB, MSB) (uint16_t)(lsb | (msb << 8))
#define GB_F_OR_AF(CTX, F_TMP) CTX->registers->F |= (F_TMP & 0xFF)
#define GB_A_OR_AF(CTX, A) CTX->registers->F |= ((A << 8) & 0XFF00)
#define GB_TMP_F() uint8_t tmpRegF = 0x00
#define GB_SET_F(FLAG, VALUE) tmpRegF |= ((VALUE) << FLAG)
#define GB_TEST_F(CTX, FLAG) ((CTX->registers->F >> FLAG) & 0X01)

typedef struct 
{
    // BIG MF TODO: FIX THIS FUCKING UNION AND AVOID USING FILE_16[] AS MAIN REGISTER ACCESS
     union {
        struct {
            uint8_t B;
            uint8_t C;
            uint8_t D;
            uint8_t E;
            uint8_t H;
            uint8_t L;
            uint8_t A;
            uint8_t F;
        };
        struct {
            uint16_t BC;
            uint16_t DE;
            uint16_t HL;
            uint16_t AF;
        };
        uint8_t  FILE_8[8];
        uint16_t FILE_16[4];
    };
    uint16_t SP;
    uint16_t PC;
    uint16_t INSTRUCTION;
} GB_Registers;

#endif