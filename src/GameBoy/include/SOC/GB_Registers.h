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

// ---------------------------- USED
#define GB_IE_REGISTER 0xFFFE
#define GB_IF_REGISTER 0xFF0F

// LCD
#define GB_LCDC_REGISTER 0xFF40 // (LCD Control Register)
#define GB_STAT_REGISTER 0xFF41 // (LCDC Status Register)

#define GB_SCY_REGISTER 0xFF42  // (Scroll Y)
#define GB_SCX_REGISTER 0xFF43  // (Scroll X)
#define GB_LY_REGISTER 0xFF44   // (LCDC Y-Coordinate)
#define GB_LYC_REGISTER 0xFF45  // (LY Compare)
#define GB_DMA_REGISTER 0xFF46  // (Direct Memory Access)
#define GB_BGP_REGISTER 0xFF47  // (Background Palette Data)
#define GB_OBP0_REGISTER 0xFF48 // (Object Palette 0 Data)
#define GB_OBP1_REGISTER 0xFF49 // (Object Palette 1 Data)
#define GB_WY_REGISTER 0xFF4A   // (Window Y Position)
#define GB_WX_REGISTER 0xFF4B   // (Window X Position - 7)

// ---------------------------- NOT USED
#define GB_HALT_REGISTER 0xFF50



// ---------------------------- Hardware registers structs

typedef struct
{
    union
    {
        struct
        {
            uint8_t IF_VBLANK  : 1;
            uint8_t IF_LCD     : 1;
            uint8_t IF_TIMER   : 1;
            uint8_t IF_SERIAL  : 1;
            uint8_t IF_JOYPAD  : 1;
        };
        uint8_t value; // Access entire register
    };
} GB_IF_register;

typedef struct
{
    union
    {
        struct
        {
            uint8_t IE_VBLANK  : 1;
            uint8_t IE_LCD     : 1;
            uint8_t IE_TIMER   : 1;
            uint8_t IE_SERIAL  : 1;
            uint8_t IE_JOYPAD  : 1;
        };
        uint8_t IE_reg; // Access entire register
    };
} GB_IE_register;

// GB_LCDC_REGISTER (LCD Control Register)
typedef struct
{
    union
    {
        struct
        {
            uint8_t LCD_DISPLAY_ENABLE : 1;
            uint8_t WINDOW_TILE_MAP_SELECT : 1;
            uint8_t WINDOW_DISPLAY_ENABLE : 1;
            uint8_t BG_WINDOW_TILE_DATA_SELECT : 1;
            uint8_t BG_TILE_MAP_SELECT : 1;
            uint8_t OBJ_SIZE : 1;
            uint8_t OBJ_DISPLAY_ENABLE : 1;
            uint8_t BG_WINDOW_DISPLAY_PRIORITY : 1;
        };
        uint8_t value; // Access entire register
    };
} GB_LCDC_Register;

// GB_STAT_REGISTER
typedef struct
{
    union
    {
        struct
        {
            uint8_t LYC_LY_COINCIDENCE_INTERRUPT : 1;
            uint8_t MODE_2_OAM_INTERRUPT : 1;
            uint8_t MODE_1_VBLANK_INTERRUPT : 1;
            uint8_t MODE_0_HBLANK_INTERRUPT : 1;
            uint8_t LYC_LY_COINCIDENCE_FLAG : 1;
            uint8_t MODE_FLAG : 2;
            uint8_t COINCIDENCE_FLAG : 1;
        };
        uint8_t value; // Access entire register
    };
} GB_STAT_Register;

#endif