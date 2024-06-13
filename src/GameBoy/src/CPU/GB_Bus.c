#include <CPU/GB_Bus.h>

/* GB_Bus.c TODOS
- IMPLEMENT HARDWARE REGISTER HASH LIST TO CHECK ACCESS POLICY (R, R/W,TRIGGER ON W, ETC)
- IMPLEMENT HARDWARE REGISTERS.... LOL
- MEMORY ACCESS SHOULD NOT BE MULTI THREADED...
*/

uint8_t GB_BusRead(EmulationState *ctx, uint16_t address)
{
    // Simple memory access (no memory hardware map) ONLY FOR CPU TESTING....
    return ctx->memory[address]; // lol...
}

void GB_BusWrite(EmulationState *ctx, uint16_t address, uint8_t value)
{
    ctx->memory[address] = value;
}

void GB_SetReg8(EmulationState *ctx, uint8_t r, uint8_t value)
{
    if (r == GB_HL_INDIRECT_OFFSET)
    {
        GB_BusWrite(ctx, ctx->registers->HL, value);
        return;
    }

    // improve (this is to ignore F register in the indexing)
    if (r == GB_A_OFFSET)
    {
        r--;
    }

    ctx->registers->FILE_8[r] = value;
}
uint8_t GB_GetReg8(EmulationState *ctx, uint8_t r)
{
    if (r == GB_HL_INDIRECT_OFFSET)
    {
        return GB_BusRead(ctx, ctx->registers->HL);
    }

    // improve (this is to ignore F register in the indexing)
    if (r == GB_A_OFFSET)
    {
        r--;
    }

    return ctx->registers->FILE_8[r];
}

// TODO: MODE IS NOT NECESSARY
void GB_SetReg16(EmulationState *ctx, uint8_t r, uint16_t value, uint8_t mode)
{
    if ((REG16_MODE_AF & mode) == 1)
    {
        ctx->registers->FILE_16[r] = value;
        return;
    }
    else
    {
        ctx->registers->SP = value;
    }
}

// TODO: MODE IS NOT NECESSARY
uint16_t GB_GetReg16(EmulationState *ctx, uint8_t r, uint8_t mode)
{
    return (REG16_MODE_AF & mode) ? ctx->registers->FILE_16[r] : ctx->registers->SP;
}