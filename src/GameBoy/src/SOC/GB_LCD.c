#include <SOC/GB_LCD.h>

// 160 SEGMENTS AT 108.7 micro seconds.
// 144 LINES AT 15.66 milli seconds
// 10  LINES AT 1.09 ms (VERTICAL BLAKING PERIDO)
// FRAME FREQ 59.7 hz

// min horizontal dots: 160 dots (20 blocks)
// min vertical 144 dots (18 blocks)

// max horizontal dots: 256 dots (32 blocks)
// max vertical 256 dots (32 blocks)

// state->ppuMode 2 oam scan: 80 dots
// state->ppuModee 3 drawing pixels: 172-289 dots
// state->ppuMode 0: 87-204 dots

//OK FORGIVE ME ABOUT THE STATICS HERE (MIGHT BE NEEDED WHEN EMBEED)

void GB_LCD_Init()
{

}

void GB_LCD_Tick(EmulationState* state, uint8_t cycles)
{
    state->ppuCycles += cycles;
    switch (state->ppuMode) {
        case 0: // HBlank
            if (state->ppuCycles>= 204) {
                state->ppuCycles-= 204;
                
                state->registers->LCD_LY++;

                if (state->registers->LCD_LY == 144) {
                    state->ppuMode = 1; // VBlank
                    // Trigger VBlank interrupt
                } else {
                    state->ppuMode = 2; // OAM search
                }
            }
            break;
        case 1: // VBlank
            if (state->ppuCycles>= 456) {
                state->ppuCycles-= 456;
                state->registers->LCD_LY++;

                if (state->registers->LCD_LY > 153) {
                    state->registers->LCD_LY = 0;
                    state->ppuMode = 2; // OAM search
                }
            }
            break;
        case 2: // OAM search
            if (state->ppuCycles>= 80) {
                state->ppuCycles-= 80;
                state->ppuMode = 3; // Drawing pixels
            }
            break;
        case 3: // Drawing pixels
            if (state->ppuCycles >= 172) {
                state->ppuCycles-= 172;
                GB_RenderScanLine();
                state->ppuMode = 0; // HBlank
            }
            break;
    }
}

void GB_RenderScanLine()
{
}

void GB_DrawBackground()
{
}

void GB_DrawWindow()
{
}

void GB_DrawObjects()
{
}
