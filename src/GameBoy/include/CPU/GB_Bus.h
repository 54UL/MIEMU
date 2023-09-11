#ifndef GB_BUS_H
#define GB_BUS_H
#include <Emulation/GB_SystemContext.h>

uint8_t GB_BusRead(SystemContext *ctx, uint16_t address);
void GB_BusWrite(SystemContext *ctx, uint16_t address, uint8_t value);

// For 8 bit registers
#define REG8_MODE_HL 0
#define REG8_MODE_A 1
// For 16 bit registers
#define REG16_MODE_AF 0
#define REG16_MODE_SP 1

// TODO: MOVE REGISTER OPERATIONS TO ANOTHER FILE...
//  8 bit reg (indexed as: [B,C,D,E,H,L,(HL),A] or [B,E,D,E,H,L,A,F]
//  16 bit reg (indexed as: [BC,DE,HL,AF] OR [BC,DE,HL,SP]

void GB_SetReg8(SystemContext *ctx, uint8_t r, uint8_t value, uint8_t &mode = 0);
uint8_t GB_GetReg8(SystemContext *ctx, uint8_t r, uint8_t &mode = 0);
void GB_SetReg16(SystemContext *ctx, uint8_t r, uint16_t value, uint8_t &mode = 0);
uint16_t GB_GetReg16(SystemContext *ctx, uint8_t r, uint8_t &mode = 0);

#endif