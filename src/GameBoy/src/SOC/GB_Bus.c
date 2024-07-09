#include <SOC/GB_Bus.h>
#include <minemu/MNE_Log.h>

/* GB_Bus.c TODOS
- FIX THE IF-ELSE HELL
*/

uint8_t GB_InAddressRange(const uint16_t a, const uint16_t b, const uint16_t addrr)
{
    // TODO:: ADD ASSERTS IF A > B AND B < A :)
    return (addrr >= a) && (addrr <= b);
}

uint8_t GB_BusRead(const EmulationState *ctx, uint16_t address)
{
    if (GB_InAddressRange(GB_BANK_00_START, GB_BANK_00_END, address))
    {
        return ctx->bios_enabled ? ctx->bios[address] : ctx->bank_00[address]; // lol...
    }
    else if (GB_InAddressRange(GB_BANK_NN_START, GB_BANK_NN_END, address))
    {
        // MB0 GOES HERE...
        MNE_Log("Not implemented readable GB_BANK_NN %04x\n", address);
    }
    else if (GB_InAddressRange(GB_VRAM_START, GB_VRAM_END, address))
    {
        return ctx->vram[GB_VRAM_START - address];
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
        MNE_Log("Warning, reading from GB_NOT_USABLE_RAM ... %04x\n", address);
        return 0xFF;
    }
    else if (GB_InAddressRange(GB_IO_START, GB_IO_END, address))
    {
        if (address == GB_IF_REGISTER) // IF REGISTER
        {
            return ctx->registers->IF.value;
        }
        else if (address == GB_LCDC_REGISTER) // LCD REGISTER
        {
            return ctx->registers->LCD_CONTROL.value;
        }
        else if (address == GB_LCD_STAT_REGISTER) // LCD REGISTER
        {
            return ctx->registers->LCD_STAT.value;
        }
        else
        {
            MNE_Log("IO RANGE NOT READBLE (NOT IMPLEMENTED!!!) io address: 0x%04x\n", address);
        }
    }
    else if (GB_InAddressRange(GB_HRAM_START, GB_HRAM_END, address))
    {
        return ctx->hram[GB_HRAM_START - address];
    }
    // INDIVIDUAL ADDRESSES (TODO: FIX THIS TRASH)
    else if (address == GB_IE_REGISTER)
    {
        return ctx->registers->IE.value;
    }
    else
    {
        MNE_Log("Undefined read access at: %04x\n", address);
    }

    return 0;
}

void GB_BusWrite(const EmulationState *ctx, uint16_t address, uint8_t value)
{
    if (GB_InAddressRange(GB_BANK_00_START, GB_BANK_00_END, address))
    {
        // BIOS READ (TODO CHECK BOOT ROM REGISTER TO DISABLE)
        if (ctx->bios_enabled && address <= 0xFF)
        {
            MNE_Log("Fool u cannot write in this region while the bios is enabled... addrr: %04x\n", address);
            return;
        }

        ctx->bank_00[address] = value;
    }
    else if (GB_InAddressRange(GB_BANK_NN_START, GB_BANK_NN_END, address))
    {
        // MB0 GOES HERE...
        MNE_Log("Not implemented writable GB_BANK_NN %04x\n", address);
    }
    else if (GB_InAddressRange(GB_VRAM_START, GB_VRAM_END, address))
    {
        ctx->vram[GB_VRAM_START - address] = value;
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
        MNE_Log("Not writable GB_NOT_USABLE_RAM range %04x\n", address);
    }
    else if (GB_InAddressRange(GB_IO_START, GB_IO_END, address))
    {
        if (address == GB_IF_REGISTER) // IF REGISTER
        {
            ctx->registers->IF.value = value;
        }
        else if (address == GB_LCDC_REGISTER) // LCD REGISTER
        {
            ctx->registers->LCD_CONTROL.value = value;
        }
        else if (address == GB_LCD_STAT_REGISTER) // LCD REGISTER
        {
            ctx->registers->LCD_STAT.value = value;
        }
        else
        {
            MNE_Log("IO RANGE NOT WRITABLE (NOT IMPLEMENTED!!!)  io address: 0x%04x\n", address);
        }
    }
    else if (GB_InAddressRange(GB_HRAM_START, GB_HRAM_END, address))
    {
        ctx->hram[GB_HRAM_START - address] = value;
    }
    else if (address == GB_IE_REGISTER) // IE REGISTER
    {
      ctx->registers->IE.value = value;
    }
    else
    {
        MNE_Log("Undefined writable access at: %04x\n", address);
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
    // TODO: IMPROVE CONDITIONS

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
        // TODO:IMPLEMENT ME MF
        break;

    default:
        MNE_Log("ERROR: GB_SetReg16 CANNOT DEDEUCE REGISTER ADDRESSING MODE");
        break;
    }
}

uint16_t GB_GetReg16(EmulationState *ctx, uint8_t r, uint8_t mode)
{
    // TODO: IMPROVE CONDITIONS
    switch (mode)
    {
    case REG16_MODE_AF:
        return ctx->registers->FILE_16[r];
    case REG16_MODE_SP:
        if (r == 3)
        {
            return ctx->registers->SP;
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