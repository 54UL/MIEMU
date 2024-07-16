#ifndef GB_LCD_HPP
#define GB_LCD_HPP

#include <Emulation/GB_SystemContext.h>


// 160 SEGMENTS AT 108.7 micro seconds.
// 144 LINES AT 15.66 milli seconds
// 10  LINES AT 1.09 ms (VERTICAL BLAKING PERIDO)
// FRAME FREQE 59.7 hz
// 
// min horizontal dots: 160 dots (20 blocks)
// min vertical         144 dots (18 blocks)

// max horizontal dots: 256 dots (32 blocks)
// max vertical         256 dots (32 blocks)

// mode 2 oam scan:        80 dots
// modee 3 drawing pixels: 172-289 dots
// mode                    0: 87-204 dots

#define GB_DISPLAY_WIDHT 160
#define GB_DISPLAY_HEIGHT 144

/*
    Initialize LCD Control (LCDC) Register (0xFF40):

    Enable BG display (Bit 0), OBJ display (Bit 1), set OBJ size (Bit 2).
    Set Up Scroll Positions:

    SCY (0xFF42): Set vertical scroll position.
    SCX (0xFF43): Set horizontal scroll position.
    Load BG Tile Map:

    Memory at 0x9800-0x9BFF: Load tile map data for background.
    Load BG Tile Data:

    Memory at 0x8800-0x97FF: Load tile graphics data for background tiles.
    Set Up Background Palette:

    BGP (0xFF47): Define colors for background tiles using palette register.
    Handle LCD Status (STAT) and Interrupts:

    STAT (0xFF41): Monitor status flags (mode bits) for VBlank and other events.
    IE (0xFFFF): Enable STAT interrupts.
    IF (0xFF0F): Handle STAT interrupt flag.
    Manage DMA Transfers (Optional for OAM):

    DMA (0xFF46): Initiate DMA transfer to OAM (0xFE00-0xFE9F) for sprite data.

*/

void GB_LCD_Init();
void GB_LCD_Tick(EmulationState* state, uint8_t cycles);

void GB_RenderScanLine(EmulationState* state);
void GB_DrawBackground();
void GB_DrawWindow();
void GB_DrawObjects();


#endif