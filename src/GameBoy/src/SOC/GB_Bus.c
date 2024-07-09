#include <SOC/GB_Bus.h>
#include <minemu/MNE_Log.h>

/* GB_Bus.c TODOS
- IMPLEMENT HARDWARE REGISTERS.... LOL
- MEMORY ACCESS SHOULD NOT BE MULTI THREADED...
*/

uint8_t GB_InAddressRange(const uint16_t a, const uint16_t b, const uint16_t addrr)
{
    // TODO:: ADD ASSERTS IF A > B AND B < A :)
    return (a >= addrr) && (addrr <= b);
} 

uint8_t GB_BusRead(EmulationState *ctx, uint16_t address)
{
    // TODO: ONLY USING IMPLEMENTED ADDRESES....
    if (GB_InAddressRange(GB_BANK_00_START, GB_BANK_00_END, address))
    {
        // BIOS READ (TODO CHECK BOOT ROM REGISTER TO DISABLE)
        return ctx->memory[address]; // lol...
    }
    else if (GB_InAddressRange(GB_BANK_NN_START, GB_BANK_NN_END, address))
    {
        // MB0 GOES HERE...
        return ctx->memory[address]; // lol...
    }
    else if (GB_InAddressRange(GB_VRAM_START, GB_VRAM_END, address))
    {
        // TODO: ADD VRAM READ
    }
    else if (GB_InAddressRange(GB_ERAM_START, GB_ERAM_END, address))
    {
        MNE_Log("Not implemented readable GB_ERAM %04x\n", address);

    }
    else if (GB_InAddressRange(GB_WRAM_START, GB_WRAM_END, address))
    {
        MNE_Log("Not implemented readable GB_WRAM %04x\n", address);
    }
    else if (GB_InAddressRange(GB_WRAM2_START, GB_WRAM2_END, address))
    {
        MNE_Log("Not implemented readable GB_WRAM2 %04x\n", address);
    }
    else if (GB_InAddressRange(GB_ECHO_RAM_START, GB_ECHO_RAM_END, address))
    {
        MNE_Log("Not implemented readable GB_ECHO_RAM %04x\n", address);
    }
    else if (GB_InAddressRange(GB_OAM_START, GB_OAM_END, address))
    {
        MNE_Log("Not implemented readable OAM %04x\n", address);
    }
    else if (GB_InAddressRange(GB_NOT_USABLE_RAM_START, GB_NOT_USABLE_RAM_END, address))
    {
        MNE_Log("Not readeable memory range %04x\n", address);
    }
    else if (GB_InAddressRange(GB_IO_START, GB_IO_END, address))
    {
        MNE_Log("Not readeable IO %04x\n", address);
    }
    else if (GB_InAddressRange(GB_HRAM_START, GB_HRAM_END, address))
    {
        MNE_Log("Not readeable GB_HRAM %04x\n", address);
    }
    else if(address == 0xFFFF) // IE REGISTER
    {
        // TODO: INSERT HERE IE REGISTER MAP
    }
    else 
    {
        MNE_Log("Undefined memory access read: addrr %04x\n", address);
    }
}

void GB_BusWrite(EmulationState *ctx, uint16_t address, uint8_t value)
{
    if (GB_InAddressRange(GB_BANK_00_START, GB_BANK_00_END, address))
    {
        // BIOS READ (TODO CHECK BOOT ROM REGISTER TO DISABLE)
        ctx->memory[address] = value;
    }
    else if (GB_InAddressRange(GB_BANK_NN_START, GB_BANK_NN_END, address))
    {
        // MB0 GOES HERE...
        ctx->memory[address] = value;
    }
    else if (GB_InAddressRange(GB_VRAM_START, GB_VRAM_END, address))
    {
        // TODO: ADD VRAM wriate
    }
    else if (GB_InAddressRange(GB_ERAM_START, GB_ERAM_END, address))
    {
        MNE_Log("Not implemented writable GB_ERAM %04x\n", address);
    }
    else if (GB_InAddressRange(GB_WRAM_START, GB_WRAM_END, address))
    {
        MNE_Log("Not implemented writable GB_WRAM %04x\n", address);
    }
    else if (GB_InAddressRange(GB_WRAM2_START, GB_WRAM2_END, address))
    {
        MNE_Log("Not implemented writable GB_WRAM2 %04x\n", address);
    }
    else if (GB_InAddressRange(GB_ECHO_RAM_START, GB_ECHO_RAM_END, address))
    {
        MNE_Log("Not implemented writable GB_ECHO_RAM %04x\n", address);
    }
    else if (GB_InAddressRange(GB_OAM_START, GB_OAM_END, address))
    {
        MNE_Log("Not implemented writable OAM %04x\n", address);
    }
    else if (GB_InAddressRange(GB_NOT_USABLE_RAM_START, GB_NOT_USABLE_RAM_END, address))
    {
        MNE_Log("Not writable memory range %04x\n", address);
    }
    else if (GB_InAddressRange(GB_IO_START, GB_IO_END, address))
    {
        MNE_Log("Not writable IO %04x\n", address);
    }
    else if (GB_InAddressRange(GB_HRAM_START, GB_HRAM_END, address))
    {
        MNE_Log("Not writable GB_HRAM %04x\n", address);
    }
    else if(address == 0xFFFF) // IE REGISTER
    {
        // TODO: INSERT HERE IE REGISTER MAP
        MNE_Log("Not writable IE Register %04x\n", address);
    }
    else 
    {
        MNE_Log("Undefined memory access read: addrr %04x\n", address);
    }
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

void GB_SetReg16(EmulationState *ctx, uint8_t r, uint16_t value, uint8_t mode)
{
    //TODO: IMPROVE CONDITIONS

    switch (mode)
    {
    case REG16_MODE_AF:
        ctx->registers->FILE_16[r] = value;
        break;

    case REG16_MODE_SP:
        if (r == 3)
        {
            ctx->registers->SP = value;
        }
        else
        {
            ctx->registers->FILE_16[r] = value;
        }
        break;

    case REG16_MODE_HL_PLUS_HL_MINUS:
        //TODO:IMPLEMENT ME MF
        break;
        
    default:
        MNE_Log("ERROR: GB_SetReg16 CANNOT DEDEUCE REGISTER ADDRESSING MODE");
        break;
    }
}

uint16_t GB_GetReg16(EmulationState *ctx, uint8_t r, uint8_t mode)
{
    //TODO: IMPROVE CONDITIONS
    switch (mode)
    {
    case REG16_MODE_AF:
        return ctx->registers->FILE_16[r];
    case REG16_MODE_SP:
        if (r == 3)
        {
            return ctx->registers->SP ;
        }
        else 
        {
            return ctx->registers->FILE_16[r];
        }
        break;
    case REG16_MODE_HL_PLUS_HL_MINUS:
        /* code */
        break; 
    default:
        MNE_Log("ERROR: GB_GetReg16 CANNOT DEDEUCE REGISTER ADDRESSING MODE");
        break;
    }
}