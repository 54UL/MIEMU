#ifndef GB_LCD_HPP
#define GB_LCD_HPP



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

void GB_LCD_Tick();
#endif