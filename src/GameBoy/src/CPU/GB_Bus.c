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
// TODO: MODE IS NOT NECESSARY
void GB_SetReg8(EmulationState *ctx, uint8_t r, uint8_t value, uint8_t mode)
{
    uint8_t *halfregs = ((uint8_t *)ctx->registers->CPU);

    if (r == GB_HL_R_INDIRECT_OFFSET)
    {
        GB_BusWrite(ctx, ctx->registers->CPU[GB_HL_OFFSET], value);
        return;
    }

    halfregs[r] = value;
}
// TODO: MODE IS NOT NECESSARY

uint8_t GB_GetReg8(EmulationState *ctx, uint8_t r, uint8_t mode)
{
    const uint8_t *halfregs = ((uint8_t *)ctx->registers->CPU);

    if (r == GB_HL_R_INDIRECT_OFFSET)
    {
        return GB_BusRead(ctx, ctx->registers->CPU[GB_HL_OFFSET]);
    }

    return halfregs[r];
}

// TODO: MODE IS NOT NECESSARY
void GB_SetReg16(EmulationState *ctx, uint8_t r, uint16_t value, uint8_t mode)
{
    if ((REG16_MODE_AF & mode) == 1)
    {
        ctx->registers->CPU[r] = value;
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
    return (REG16_MODE_AF & mode) ? ctx->registers->CPU[r] : ctx->registers->SP;
}