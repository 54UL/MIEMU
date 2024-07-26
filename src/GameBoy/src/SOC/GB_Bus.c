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

uint8_t GB_ReadIO(const GB_Registers* registers, const uint16_t address)
{
        MNE_Log("[IO READ INTENT] %04x\n", address);

        switch (address)
        {
            // CPU RELATED REGISTERS
            case GB_IF_REGISTER:
                return registers->IF.value;
            
            // DISPLAY REGISTERS
            case GB_LCDC_REGISTER:
                return registers->LCD_CONTROL.value;
            case GB_LCD_STAT_REGISTER:
                return registers->LCD_STAT.value;

            case GB_SCY_REGISTER:
                return registers->LCD_SCY;

            case GB_SCX_REGISTER:
                return registers->LCD_SCX;

            case GB_LY_REGISTER:
                return registers->LCD_LY;

            case GB_LYC_REGISTER:
                return registers->LCD_LYC;

            case GB_DMA_REGISTER:
                // return registers->LCD_DMA;
                MNE_Log("GB_DMA_REGISTER NOT READEABLE (NOT IMPLEMENTED!!!)\n");
                return 0xFF;

            case GB_BGP_REGISTER:
                return registers->LCD_BGP;

            case GB_OBP0_REGISTER:
                return registers->LCD_OBP0;

            case GB_OBP1_REGISTER:
                return registers->LCD_OBP1;

            case GB_WY_REGISTER:
                return registers->LCD_WY;

            case GB_WX_REGISTER:
                return registers->LCD_WX;

        default:
                MNE_Log("IO RANGE NOT READBLE (NOT IMPLEMENTED!!!) io address: 0x%04x\n", address);
            break;
        }

    return 0xFF;
}

void GB_WriteIO(GB_Registers* registers, const uint16_t address, const uint8_t value)
{
    MNE_Log("[IO WRITE INTENT] %04x\n", address);

    switch (address)
    {
        // CPU RELATED REGISTERS
        case GB_IF_REGISTER:
            registers->IF.value = value;
            break;
        
        // DISPLAY REGISTERS
        case GB_LCDC_REGISTER:
            registers->LCD_CONTROL.value = value;
            break;

        case GB_LCD_STAT_REGISTER:
            registers->LCD_STAT.value = value;
            break;

        case GB_SCY_REGISTER:
            registers->LCD_SCY = value;
            break;

        case GB_SCX_REGISTER:
            registers->LCD_SCX = value;
            break;

        case GB_LY_REGISTER:
            registers->LCD_LY = value;
            break;

        case GB_LYC_REGISTER:
            registers->LCD_LYC = value;
            break;

        case GB_DMA_REGISTER:
            // registers->LCD_DMA = value;
            MNE_Log("GB_DMA_REGISTER NOT WRITABLE (NOT IMPLEMENTED!!!)\n");

            break;

        case GB_BGP_REGISTER:
            registers->LCD_BGP = value;
            break;

        case GB_OBP0_REGISTER:
            registers->LCD_OBP0 = value;
            break;

        case GB_OBP1_REGISTER:
            registers->LCD_OBP1 = value;
            break;

        case GB_WY_REGISTER:
            registers->LCD_WY = value;
            break;

        case GB_WX_REGISTER:
            registers->LCD_WX = value;
            break;

    default:
        MNE_Log("IO RANGE NOT WRITABLE (NOT IMPLEMENTED!!!)  io address: 0x%04x\n", address);
        break;
    }
}

uint8_t GB_BusRead(const EmulationState *ctx, uint16_t address)
{
    if (GB_InAddressRange(GB_BANK_00_START, GB_BANK_00_END, address))
    {
        if (address > 256)
        {
            MNE_Log("BANK_00 read address [%04x];\n", address);
        }

        return ctx->bios_enabled ? ctx->bios[address] : ctx->bank_00[address]; // lol...
    }
    else if (GB_InAddressRange(GB_BANK_NN_START, GB_BANK_NN_END, address))
    {
        // MB0 GOES HERE...
        MNE_Log("Not implemented readable GB_BANK_NN %04x\n", address);
    }
    else if (GB_InAddressRange(GB_VRAM_START, GB_VRAM_END, address))
    {
        MNE_Log("VRAM READ!!! %04x\n", address);

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
        return GB_ReadIO(&ctx->registers, address);
    }
    else if (GB_InAddressRange(GB_HRAM_START, GB_HRAM_END, address))
    {
        return ctx->hram[GB_HRAM_START - address];
    }
    // INDIVIDUAL ADDRESSES (TODO: FIX THIS TRASH)
    else if (address == GB_IE_REGISTER)
    {
        return ctx->registers.IE.value;
    }
    else
    {
        MNE_Log("Undefined read access at: %04x\n", address);
    }

    return 0;
}

void GB_BusWrite(EmulationState *ctx, uint16_t address, uint8_t value)
{
    if (GB_InAddressRange(GB_BANK_00_START, GB_BANK_00_END, address))
    {
        // BIOS READ (TODO CHECK BOOT ROM REGISTER TO DISABLE)
        if (ctx->bios_enabled && address <= 0xFF)
        {
            MNE_Log("Fool u cannot write in this region while the bios is enabled... addrr: %04x\n", address);
            return;
        }
        MNE_Log("BANK_00 WRITE address [%04x]; value[%02x]\n", address, value);


        ctx->bank_00[address] = value;
    }
    else if (GB_InAddressRange(GB_BANK_NN_START, GB_BANK_NN_END, address))
    {
        // MB0 GOES HERE...
        MNE_Log("Not implemented writable GB_BANK_NN %04x\n", address);
    }
    else if (GB_InAddressRange(GB_VRAM_START, GB_VRAM_END, address))
    {
        MNE_Log("VRAM WRITE!!! %04x\n", address);

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
        GB_WriteIO(&ctx->registers, address, value);
    }
    else if (GB_InAddressRange(GB_HRAM_START, GB_HRAM_END, address))
    {
        ctx->hram[GB_HRAM_START - address] = value;
    }
    else if (address == GB_IE_REGISTER) // IE REGISTER
    {
        ctx->registers.IE.value = value;
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
        GB_BusWrite(ctx, ctx->registers.HL, value);
        return;
    }

    // improve (this is to ignore F register in the indexing)
    if (r == GB_A_OFFSET)
    {
        r--;
    }

    ctx->registers.FILE_8[r] = value;
}

uint8_t GB_GetReg8(EmulationState *ctx, uint8_t r)
{
    if (r == GB_HL_INDIRECT_OFFSET)
    {
        return GB_BusRead(ctx, ctx->registers.HL);
    }

    // improve (this is to ignore F register in the indexing)
    if (r == GB_A_OFFSET)
    {
        r--;
    }

    return ctx->registers.FILE_8[r];
}

void GB_SetReg16(EmulationState *ctx, uint8_t r, uint16_t value, uint8_t mode)
{
    // TODO: IMPROVE CONDITIONS

    switch (mode)
    {
    case REG16_MODE_AF:
        ctx->registers.FILE_16[r] = value;
        break;

    case REG16_MODE_SP:
        if (r == 3)
        {
            ctx->registers.SP = value;
        }
        else
        {
            ctx->registers.FILE_16[r] = value;
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
        return ctx->registers.FILE_16[r];
    case REG16_MODE_SP:
        if (r == 3)
        {
            return ctx->registers.SP;
        }
        else
        {
            return ctx->registers.FILE_16[r];
        }
        break;
    case REG16_MODE_HL_PLUS_HL_MINUS:
        /* code */
        break;
    default:
        MNE_Log("ERROR: GB_GetReg16 CANNOT DEDEUCE REGISTER ADDRESSING MODE");
        break;
    }
    
    return 0;
}