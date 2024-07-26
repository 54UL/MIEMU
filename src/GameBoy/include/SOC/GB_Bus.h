#ifndef GB_BUS_H
#define GB_BUS_H
#include <Emulation/GB_SystemContext.h>

// 16 BIT ADDRESSING MODES (enumerated)
#define REG16_MODE_AF 0
#define REG16_MODE_SP 1
#define REG16_MODE_HL_PLUS_HL_MINUS 2

//TODO: IMPROVE THIS METHOD....
uint8_t GB_InAddressRange(const uint16_t a, const uint16_t b, const uint16_t addrr);
uint8_t GB_BusRead(const EmulationState *ctx, uint16_t address);
void GB_BusWrite(EmulationState *ctx, uint16_t address, uint8_t value);

// TODO: MOVE REGISTER OPERATIONS TO ANOTHER FILE...
void     GB_SetReg8(EmulationState *ctx, uint8_t r, uint8_t value);
uint8_t  GB_GetReg8(EmulationState *ctx, uint8_t r);
void     GB_SetReg16(EmulationState *ctx, uint8_t r, uint16_t value, uint8_t mode);
uint16_t GB_GetReg16(EmulationState *ctx, uint8_t r, uint8_t mode);

#endif