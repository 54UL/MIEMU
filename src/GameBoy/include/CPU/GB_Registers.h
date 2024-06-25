#ifndef GB_REGISTERS_H
#define GB_REGISTERS_H

#include <stdint.h>

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

// #define GB_SET_F(DATA, FLAG, VALUE) DATA |= ((VALUE) << FLAG)
// #define GB_TEST_F(DATA, CTX, FLAG) ((DATA>> FLAG) & 0X01)

typedef struct 
{
     union {
        struct {
            uint8_t B;
            uint8_t C;
            uint8_t D;
            uint8_t E;
            uint8_t H;
            uint8_t L;
            uint8_t A;

            union
            {
                struct
                {
                    uint8_t ZERO_FLAG : GB_ZERO_FLAG;
                    uint8_t N_FLAG : GB_N_FLAG;
                    uint8_t H_CARRY_FLAG : GB_H_FLAG;
                    uint8_t CARRY_FLAG : GB_C_FLAG;
                };
                uint8_t F;
            };
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