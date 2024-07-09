#include <SOC/GB_LCD.h>

// 160 SEGMENTS AT 108.7 micro seconds.
// 144 LINES AT 15.66 milli seconds
// 10  LINES AT 1.09 ms (VERTICAL BLAKING PERIDO)
// FRAME FREQ 59.7 hz

// min horizontal dots: 160 dots (20 blocks)
// min vertical 144 dots (18 blocks)

// max horizontal dots: 256 dots (32 blocks)
// max vertical 256 dots (32 blocks)

// mode 2 oam scan: 80 dots
// modee 3 drawing pixels: 172-289 dots
// mode 0: 87-204 dots

void GB_LCD_Init()
{

}

void GB_LCD_Tick()
{

}
