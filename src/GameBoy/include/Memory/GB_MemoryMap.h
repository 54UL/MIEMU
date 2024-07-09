#ifndef GB_MEMORY_MAP_H
#define GB_MEMORY_MAP_H
// TODO: DELETE THIS FILE BC ALL DEFINITIONS OF MEMORY SHOULD BE AT REGISTERS.H
/*
# Hardware Registers (NO SOUND REGS) (NO CGB REGS)

Address    | Name        | Description                                                       | Readable / Writable | Models
-----------|-------------|-------------------------------------------------------------------|---------------------|-------
$FF00      | [P1/JOYP]   | Joypad                                                            | Mixed               | All
$FF04      | [DIV]       | Divider register                                                  | R/W                 | All
$FF05      | [TIMA]      | Timer counter                                                     | R/W                 | All
$FF06      | [TMA]       | Timer modulo                                                      | R/W                 | All
$FF07      | [TAC]       | Timer control                                                     | R/W                 | All
$FF0F      | [IF]        | Interrupt flag                                                    | R/W                 | All
$FF24      | [NR50]      | Master volume & VIN panning                                       | R/W                 | All
$FF30-FF3F | [Wave RAM]  | Storage for one of the sound channels' waveform                   | R/W                 | All
$FF40      | [LCDC]      | LCD control                                                       | R/W                 | All
$FF41      | [STAT]      | LCD status                                                        | Mixed               | All
$FF42      | [SCY]       | Viewport Y position                                               | R/W                 | All
$FF43      | [SCX]       | Viewport X position                                               | R/W                 | All
$FF44      | [LY]        | LCD Y coordinate                                                  | R                   | All
$FF45      | [LYC]       | LY compare                                                        | R/W                 | All
$FF46      | [DMA]       | OAM DMA source address & start                                    | R/W                 | All
$FF47      | [BGP]       | BG palette data                                                   | R/W                 | DMG
$FF48      | [OBP0]      | OBJ palette 0 data                                                | R/W                 | DMG
$FF49      | [OBP1]      | OBJ palette 1 data                                                | R/W                 | DMG
$FF4A      | [WY]        | Window Y position                                                 | R/W                 | All
$FF4B      | [WX]        | Window X position plus 7                                          | R/W                 | All
$FFFF      | [IE]        | Interrupt enable                                                  | R/W                 | All
*/

#define GB_BANK_00_START 0x0000
#define GB_BANK_00_END 0x3FFF
#define GB_BANK_01_START 0x4000
#define GB_BANK_NN_END 0x7FFF

#define GB_VRAM_START 0x8000
#define GB_VRAM_END 0x9FFF

#define GB_ERAM_START 0xA000
#define GB_ERAM_END 0xBFFF

#define GB_WRAM_START 0XC000
#define GB_WRAM_END 0xCFFF

#define GB_WRAM2_START 0XD000
#define GB_WRAM2_END 0xDFFF

#define GB_OAM_START 0XFE00
#define GB_OAM_END 0XFE9F

#define GB_IO_START 0xFF00
#define GB_IO_END 0xFF7F

#define GB_HRAM_START 0xFF80
#define GB_HRAM_END 0xFFFE

// actually used
#define GB_IE_REGISTER 0xFFFE
#define GB_IF_REGISTER 0xFF0F

#define GB_HALT_REGISTER 0xFF50

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

#endif