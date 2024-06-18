#include <CPU/GB_Instructions.h>
#include <CPU/GB_Bus.h>
#include <CPU/GB_Registers.h>
#include <minemu/MNE_Log.h>

/*INSTRUCTIONS TODOS:

- IMPLEMENT: DEBUG TOOLS TO PROFILE AND MESURE EXECUTIONS TIMES...
- OPTIMIZATION: REMOVE UNNECESSARY LOCALS
- OPTIMIZATION: FLAGS OPERATIONS
- OPTIMIZATION: ENSURE MINIMAL INSTRUCTION SIZE IMPLEMENTATION AND COMPILER INTRISICS FOR AVR AND STM32 (CHECK UDISPLAY PROYECT...)
*/

// 8-BIT LOAD INSTRUCTIONS
uint8_t GB_LD_R_R(EmulationState *ctx)
{
    // encoding: b01xxxyyy/various
    /*
        R = R
    */
    const uint8_t r1 = (ctx->registers->INSTRUCTION & 0x38) >> 3;
    const uint8_t r2 = (ctx->registers->INSTRUCTION & 0x07);

    GB_SetReg8(ctx, r1, GB_GetReg8(ctx, r2));

    return 4; // instruction clock cycles
}

uint8_t GB_LD_R_N(EmulationState *ctx)
{
    // encoding: 0b00xxx110/various + n
    /*
        R = read(PC++)
    */
    const uint8_t r = (ctx->registers->INSTRUCTION & 0x38) >> 3;

    GB_SetReg8(ctx, r, GB_BusRead(ctx, ctx->registers->PC++));

    return 8; // instruction clock cycles
}

uint8_t GB_LD_R_HL(EmulationState *ctx)
{
    // encoding: 0b01xxx110/various
    /*
        R = read(HL)
    */
    const uint8_t r = (ctx->registers->INSTRUCTION & 0x38) >> 3;
    GB_SetReg8(ctx, r, GB_BusRead(ctx, ctx->registers->HL));

    return 8;
}

uint8_t GB_LD_HL_R(EmulationState *ctx)
{
    // encoding: 0b01110xxx
    /*
        write(HL, R)
    */
    const uint8_t r = ctx->registers->INSTRUCTION & 0x07;

    GB_SetReg8(ctx, r, GB_BusRead(ctx, ctx->registers->HL));
    return 8;
}

uint8_t GB_LD_HL_N(EmulationState *ctx)
{
    // encoding: 0b00110110/0x36 + n
    /*
        n = read(PC++)
        write(HL, n)
    */
    const uint8_t n = GB_BusRead(ctx, ctx->registers->PC++);

    GB_BusWrite(ctx, ctx->registers->HL, n);

}

uint8_t GB_LD_A_BC(EmulationState *ctx)
{
    // encoding: 0b00001010
    /*
        A = read(BC)
    */
    ctx->registers->A = GB_BusRead(ctx, ctx->registers->BC);
}

uint8_t GB_LD_A_DE(EmulationState *ctx)
{
    // encoding: 0b00011010
    /*
        A = read(DE)
    */
   ctx->registers->A = GB_BusRead(ctx, ctx->registers->DE);
}

uint8_t GB_LD_A_NN(EmulationState *ctx)
{
    // encoding: 0b11111010/0xFA + LSB of nn + MSB of nn
    /*
        nn = unsigned_16(lsb=read(PC++), msb=read(PC++))
        A = read(nn)
    */

    const uint8_t lsb = GB_BusRead(ctx, ctx->registers->PC++);
    const uint8_t msb = GB_BusRead(ctx, ctx->registers->PC++);
    const uint16_t nn = (uint16_t)(lsb | (msb << 8));

    GB_SetReg8(ctx, GB_A_OFFSET, GB_BusRead(ctx, nn));
}

uint8_t GB_LD_BC_A(EmulationState *ctx)
{
    //encoding: 0b00000010
    /*
        write(BC, A)
    */

    //TODO: FIX ctx->registers->A for getting the A reg instead of (HL) due to register instruction index encoding
    //TODO: Also this applies for GB_SetReg8
    GB_BusWrite(ctx, ctx->registers->BC, ctx->registers->A);
}

uint8_t GB_LD_DE_A(EmulationState *ctx)
{
    //encoding: 0b00010010
    /*
        write(DE, A)
    */
    GB_BusWrite(ctx, ctx->registers->DE, ctx->registers->A);
}

uint8_t GB_LD_NN_A(EmulationState *ctx)
{
    //encoding: 0b11101010/0xEA + LSB of nn + MSB of nn
    /*
        nn = unsigned_16(lsb=read(PC++), msb=read(PC++))
        write(nn, A)
    */
    const uint8_t lsb = GB_BusRead(ctx, ctx->registers->PC++);
    const uint8_t msb = GB_BusRead(ctx, ctx->registers->PC++);
    const uint16_t nn = (uint16_t)(lsb | (msb << 8));

    GB_BusWrite(ctx, nn,  ctx->registers->A);
}

uint8_t GB_LDH_A_N(EmulationState *ctx)
{
    //encoding: 0b11110000
    /*
        n = read(PC++)
        A = read(unsigned_16(lsb=n, msb=0xFF))
    */

    const uint8_t n = GB_BusRead(ctx, ctx->registers->PC++);
    const uint16_t u16 = (uint16_t)(n | (0xFF << 8));

    GB_SetReg8(ctx, GB_A_OFFSET, GB_BusRead(ctx, u16));
}

uint8_t GB_LDH_N_A(EmulationState *ctx)
{
    //encoding: 0b11100000
    /*
        n = read(PC++)
        write(unsigned_16(lsb=n, msb=0xFF), A)
    */

    const uint8_t n = GB_BusRead(ctx, ctx->registers->PC++);
    const uint16_t u16 = (uint16_t)(n | (0xFF << 8));

    GB_BusWrite(ctx, u16,  ctx->registers->A);
}

uint8_t GB_LDH_A_C(EmulationState *ctx)
{
    // encoding: 0b11110010
    /*
        A = read(unsigned_16(lsb=C, msb=0xFF))
    */
    const uint16_t u16 = (uint16_t)(ctx->registers->C | (0xFF << 8));
    GB_SetReg8(ctx, GB_A_OFFSET, GB_BusRead(ctx, u16));
}

uint8_t GB_LDH_C_A(EmulationState *ctx)
{
    // encoding: 0b11100010
    /*
        write(unsigned_16(lsb=C, msb=0xFF), A)
    */
    const uint16_t u16 = (uint16_t)(ctx->registers->C | (0xFF << 8));
    GB_BusWrite(ctx, u16, ctx->registers->A);
}

uint8_t GB_LDI_HL_A(EmulationState *ctx)
{
    // encoding: 0b00100010
    /*
        write(HL++, A)
    */
    GB_BusWrite(ctx, ctx->registers->HL++, ctx->registers->A);
}

uint8_t GB_LDI_A_HL(EmulationState *ctx)
{
    // encoding: 0b00101010
    /*
        A = read(HL++)
    */
    GB_SetReg8(ctx, GB_A_OFFSET, GB_BusRead(ctx, ctx->registers->HL++));
}

uint8_t GB_LDD_HL_A(EmulationState *ctx)
{
    // encoding: 0b00110010
    /*
        write(HL--, A)
    */

    GB_BusWrite(ctx, ctx->registers->HL--, ctx->registers->A);
}

uint8_t GB_LDD_A_HL(EmulationState *ctx)
{
    // encoding: 0b00111010
    /*
        A = read(HL--)
    */
    GB_SetReg8(ctx, GB_A_OFFSET, GB_BusRead(ctx, ctx->registers->HL--));
}

// 16 BIT LOAD INSTRUCTIONS
uint8_t GB_LD_RR_NN(EmulationState *ctx)
{
    //encoding: 0b00xx0001, xx : 0x30
    /*
        nn = unsigned_16(lsb=read(PC++), msb=read(PC++))
        rr = nn
    */
    // TODO: SET ENC REG DOES NOT WORK FOR THIS INSTRUCTION DUE TO RR == 3 IS SP AND NOT AF
    const uint8_t rr = (ctx->registers->INSTRUCTION & 0x30) >> 4;
    const uint8_t lsb = GB_BusRead(ctx, ctx->registers->PC++);
    const uint8_t msb = GB_BusRead(ctx, ctx->registers->PC++);
    const uint16_t nn = (uint16_t)(lsb | (msb << 8));
    
    GB_SetReg16(ctx, rr, nn, REG16_MODE_SP);
}

uint8_t GB_LD_NN_SP(EmulationState *ctx)
{
    //encoding: 0b00001000
    /*
        nn_lsb = read_memory(addr=PC); PC = PC + 1
        nn_msb = read_memory(addr=PC); PC = PC + 1
        nn = unsigned_16(lsb=nn_lsb, msb=nn_msb)
        write_memory(addr=nn, data=lsb(SP)); nn = nn + 1
        write_memory(addr=nn, data=msb(SP))
    */

    const uint8_t lsb = GB_BusRead(ctx, ctx->registers->PC++);
    const uint8_t msb = GB_BusRead(ctx, ctx->registers->PC++);
    
    const uint16_t indirect_addr = (uint16_t)(lsb | (msb << 8));

    const uint8_t spl = ctx->registers->SP & 0xFF;
    const uint8_t sph = (ctx->registers->SP >> 8) & 0xFF;

    GB_BusWrite(ctx, indirect_addr, spl);
    GB_BusWrite(ctx, indirect_addr + 1, sph);
}

uint8_t GB_LD_SP_HL(EmulationState *ctx)
{
    // encoding:  0b11111001
    /*
        SP = HL
    */
    ctx->registers->SP = ctx->registers->HL;
}

uint8_t GB_PUSH_RR(EmulationState *ctx)
{
    //encoding: 0b11xx0101,xx : 0x30
    /*
        SP--
        write(SP--, msb(BC))
        write(SP, lsb(BC))
    */
    const uint8_t rr = (ctx->registers->INSTRUCTION & 0x30) >> 4;
    uint16_t drr = GB_GetReg16(ctx, rr, REG16_MODE_SP);
    const uint8_t l = drr & 0xFF;
    const uint8_t h = (drr >> 8) & 0xFF;

    ctx->registers->SP--;
    GB_BusWrite(ctx, ctx->registers->SP--, h);
    GB_BusWrite(ctx, ctx->registers->SP, l);
}

uint8_t GB_POP_RR(EmulationState *ctx)
{
    // encoding: 0b11xx0001
    /*
        BC = unsigned_16(lsb=read(SP++), msb=read(SP++))
    */
    const uint8_t rr = (ctx->registers->INSTRUCTION & 0x30) >> 4;
    
    const uint8_t l =  GB_BusRead(ctx,ctx->registers->SP++);
    const uint8_t h =  GB_BusRead(ctx,ctx->registers->SP++);
    GB_SetReg16(ctx, rr,  l | (h << 8), REG16_MODE_AF);
}

// 8 BIT ALU INSTRUCTIONS
uint8_t GB_ADD_A_R(EmulationState *ctx)
{
    // encoding: 0b10000xxx
    /*
        result, carry_per_bit = A + R (example)
        A = result
        flags.Z = 1 if result == 0 else 0
        flags.N = 0
        flags.H = 1 if carry_per_bit[3] else 0
        flags.C = 1 if carry_per_bit[7] else 0
    */
    const uint8_t rr = (ctx->registers->INSTRUCTION & 0x07);
    const uint8_t a = ctx->registers->A;
    const uint8_t r = GB_GetReg8(ctx, rr);

    const uint16_t sum =  a + r;

    ctx->registers->A = sum;

    GB_TMP_F();
    GB_SET_F(GB_ZERO_FLAG, sum == 0);
    GB_SET_F(GB_N_FLAG, 0);
    GB_SET_F(GB_H_FLAG, (sum & 0x0F) > 0x0F);
    GB_SET_F(GB_C_FLAG, sum > 0xFF);
    
    //Used to set F reg 
    GB_F_OR_AF(ctx, tmpRegF);
    
    return 4;
}

uint8_t GB_ADD_A_N(EmulationState *ctx)
{
    // encoding: 0b11000110
    /*
        n = read(PC++)
        result, carry_per_bit = A + n
        A = result
        flags.Z = 1 if result == 0 else 0
        flags.N = 0
        flags.H = 1 if carry_per_bit[3] else 0
        flags.C = 1 if carry_per_bit[7] else 0
    */

    const uint8_t n = GB_BusRead(ctx, ctx->registers->PC++);
    const uint16_t sum =  ctx->registers->A + n;
    ctx->registers->A = sum;
    
    GB_TMP_F();
    GB_SET_F(GB_ZERO_FLAG, sum == 0);
    GB_SET_F(GB_N_FLAG, 0);
    GB_SET_F(GB_H_FLAG, (sum & 0x0F) > 0x0F);
    GB_SET_F(GB_C_FLAG, sum > 0xFF);
    
    //Used to set F reg 
    GB_F_OR_AF(ctx, tmpRegF);
}

uint8_t GB_ADD_A_HL(EmulationState *ctx)
{
    // encoding: 0b10000110
    /*
        data = read(HL)
        result, carry_per_bit = A + data
        A = result
        flags.Z = 1 if result == 0 else 0
        flags.N = 0
        flags.H = 1 if carry_per_bit[3] else 0
        flags.C = 1 if carry_per_bit[7] else 0
    */

    const uint8_t n = GB_BusRead(ctx, ctx->registers->HL);
    const int sum =  ctx->registers->A + n;
    ctx->registers->A = sum;

    GB_TMP_F();
    GB_SET_F(GB_ZERO_FLAG, sum == 0);
    GB_SET_F(GB_N_FLAG, 0);
    GB_SET_F(GB_H_FLAG, (sum & 0x0F) > 0x0F);
    GB_SET_F(GB_C_FLAG, sum > 0xFF);
    
    //Used to set F reg 
    GB_F_OR_AF(ctx, tmpRegF);

    return 8;
}

uint8_t GB_ADC_A_R(EmulationState *ctx)
{
    //encoding:0b10001xxx
    /*
        result, carry_per_bit = A + flags.C + B
        A = result
        flags.Z = 1 if result == 0 else 0
        flags.N = 0
        flags.H = 1 if carry_per_bit[3] else 0
        flags.C = 1 if carry_per_bit[7] else 0
    */

    const uint8_t rr = (ctx->registers->INSTRUCTION & 0x07);
    const uint8_t c = GB_TEST_F(ctx, GB_C_FLAG);

    const uint16_t sum =  ctx->registers->A + c + GB_GetReg8(ctx, rr);
    ctx->registers->A = sum;

    GB_TMP_F();
    GB_SET_F(GB_ZERO_FLAG, sum == 0);
    GB_SET_F(GB_N_FLAG, 0);
    GB_SET_F(GB_H_FLAG, (sum & 0x0F) > 0x0F);
    GB_SET_F(GB_C_FLAG, sum > 0xFF);
    
    //Used to set F reg 
    GB_F_OR_AF(ctx, tmpRegF);
    
    return 4;
}

uint8_t GB_ADC_A_N(EmulationState *ctx)
{
    //encoding: 0b11001110
    /*
        n = read(PC++)
        result, carry_per_bit = A + flags.C + n
        A = result
        flags.Z = 1 if result == 0 else 0
        flags.N = 0
        flags.H = 1 if carry_per_bit[3] else 0
        flags.C = 1 if carry_per_bit[7] else 0
    */

    const uint8_t n = GB_BusRead(ctx, ctx->registers->PC++);
    const uint8_t c = GB_TEST_F(ctx, GB_C_FLAG);

    const int sum =  ctx->registers->A + c + n;
    ctx->registers->A = sum;

    GB_TMP_F();
    GB_SET_F(GB_ZERO_FLAG, sum == 0);
    GB_SET_F(GB_N_FLAG, 0);
    GB_SET_F(GB_H_FLAG, (sum & 0x0F) > 0x0F);
    GB_SET_F(GB_C_FLAG, sum > 0xFF);
    
    //Used to set F reg 
    GB_F_OR_AF(ctx, tmpRegF);

    return 8;
}

uint8_t GB_ADC_A_HL(EmulationState *ctx)
{
    //encoding: 0b10001110
    /*
        data = read(HL)
        result, carry_per_bit = A + flags.C + data
        A = result
        flags.Z = 1 if result == 0 else 0
        flags.N = 0
        flags.H = 1 if carry_per_bit[3] else 0
        flags.C = 1 if carry_per_bit[7] else 0
    */

    const uint8_t n = GB_BusRead(ctx, ctx->registers->HL);
    const uint8_t c = GB_TEST_F(ctx, GB_C_FLAG);

    const int sum =  ctx->registers->A + c + n;
    ctx->registers->A = sum;

    GB_TMP_F();
    GB_SET_F(GB_ZERO_FLAG, sum == 0);
    GB_SET_F(GB_N_FLAG, 0);
    GB_SET_F(GB_H_FLAG, (sum & 0x0F) > 0x0F);
    GB_SET_F(GB_C_FLAG, sum > 0xFF);
    
    //Used to set F reg 
    GB_F_OR_AF(ctx, tmpRegF);

    return 8;
}

uint8_t GB_SUB_R(EmulationState *ctx)
{
    // encoding: 0b10010xxx
    /*
        result, carry_per_bit = A - B
        A = result
        flags.Z = 1 if result == 0 else 0
        flags.N = 1
        flags.H = 1 if carry_per_bit[3] else 0
        flags.C = 1 if carry_per_bit[7] else 0
    */

    const uint8_t rr = (ctx->registers->INSTRUCTION & 0x07);
    const int sum =  ctx->registers->A - GB_GetReg8(ctx, rr);
    ctx->registers->A = sum;

    GB_TMP_F();
    GB_SET_F(GB_ZERO_FLAG, sum == 0);
    GB_SET_F(GB_N_FLAG, 1);
    GB_SET_F(GB_H_FLAG, (sum & 0x0F) > 0x0F);
    GB_SET_F(GB_C_FLAG, sum > 0xFF);
    
    //Used to set F reg 
    GB_F_OR_AF(ctx, tmpRegF);
}

uint8_t GB_SUB_N(EmulationState *ctx)
{
    // encoding: 0b11010110
    /*
        n = read(PC++)
        result, carry_per_bit = A - n
        A = result
        flags.Z = 1 if result == 0 else 0
        flags.N = 1
        flags.H = 1 if carry_per_bit[3] else 0
        flags.C = 1 if carry_per_bit[7] else 0
    */

    const uint8_t n = GB_BusRead(ctx, ctx->registers->PC++);
    const int sum =  ctx->registers->A - n;
    ctx->registers->A = sum;

    GB_TMP_F();
    GB_SET_F(GB_ZERO_FLAG, sum == 0);
    GB_SET_F(GB_N_FLAG, 1);
    GB_SET_F(GB_H_FLAG, (sum & 0x0F) > 0x0F);
    GB_SET_F(GB_C_FLAG, sum > 0xFF);
    
    //Used to set F reg 
    GB_F_OR_AF(ctx, tmpRegF);
}

uint8_t GB_SUB_HL(EmulationState *ctx)
{
    // encoding: 0b10010110
    /*
        data = read(HL)
        result, carry_per_bit = A - data
        A = result
        flags.Z = 1 if result == 0 else 0
        flags.N = 1
        flags.H = 1 if carry_per_bit[3] else 0
        flags.C = 1 if carry_per_bit[7] else 0
    */

    const uint8_t n = GB_BusRead(ctx, ctx->registers->HL);
    const int sum =  ctx->registers->A - n;
    ctx->registers->A = sum;

    GB_TMP_F();
    GB_SET_F(GB_ZERO_FLAG, sum == 0);
    GB_SET_F(GB_N_FLAG, 1);
    GB_SET_F(GB_H_FLAG, (sum & 0x0F) > 0x0F);
    GB_SET_F(GB_C_FLAG, sum > 0xFF);
    
    //Used to set F reg 
    GB_F_OR_AF(ctx, tmpRegF);
}

uint8_t GB_SBC_A_R(EmulationState *ctx)
{
    // encoding: 0b10011xxx
    /*
        result, carry_per_bit = A - flags.C - B
        A = result
        flags.Z = 1 if result == 0 else 0
        flags.N = 1
        flags.H = 1 if carry_per_bit[3] else 0
        flags.C = 1 if carry_per_bit[7] else 0
    */
    const uint8_t rr = (ctx->registers->INSTRUCTION & 0x07);
    const uint8_t c = GB_TEST_F(ctx, GB_C_FLAG);

    const int sum =  ctx->registers->A - c - GB_GetReg8(ctx, rr);
    ctx->registers->A = sum;

    GB_TMP_F();
    GB_SET_F(GB_ZERO_FLAG, sum == 0);
    GB_SET_F(GB_N_FLAG, 1);
    GB_SET_F(GB_H_FLAG, (sum & 0x0F) > 0x0F);
    GB_SET_F(GB_C_FLAG, sum > 0xFF);
    
    //Used to set F reg 
    GB_F_OR_AF(ctx, tmpRegF);
}

uint8_t GB_SBC_A_N(EmulationState *ctx)
{
    // encoding: 0b11011110
    /*
        n = read(PC++)
        result, carry_per_bit = A - flags.C - n
        A = result
        flags.Z = 1 if result == 0 else 0
        flags.N = 1
        flags.H = 1 if carry_per_bit[3] else 0
        flags.C = 1 if carry_per_bit[7] else 0
    */

    const uint8_t n = GB_BusRead(ctx, ctx->registers->PC++);
    const uint8_t c = GB_TEST_F(ctx, GB_C_FLAG);
    const int sum =  ctx->registers->A - c - n;
    ctx->registers->A = sum;

    GB_TMP_F();
    GB_SET_F(GB_ZERO_FLAG, sum == 0);
    GB_SET_F(GB_N_FLAG, 1);
    GB_SET_F(GB_H_FLAG, (sum & 0x0F) > 0x0F);
    GB_SET_F(GB_C_FLAG, sum > 0xFF);
    
    //Used to set F reg 
    GB_F_OR_AF(ctx, tmpRegF);
}

uint8_t GB_SBC_A_HL(EmulationState *ctx) 
{
    // encoding: 0b10011110
    /*
        data = read(HL)
        result, carry_per_bit = A - flags.C - data
        A = result
        flags.Z = 1 if result == 0 else 0
        flags.N = 1
        flags.H = 1 if carry_per_bit[3] else 0
        flags.C = 1 if carry_per_bit[7] else 0
    */

    const uint8_t n = GB_BusRead(ctx, ctx->registers->HL);
    const uint8_t c = GB_TEST_F(ctx, GB_C_FLAG);

    const int sum =  ctx->registers->A - c - n;
    ctx->registers->A = sum;

    GB_TMP_F();
    GB_SET_F(GB_ZERO_FLAG, sum == 0);
    GB_SET_F(GB_N_FLAG, 1);
    GB_SET_F(GB_H_FLAG, (sum & 0x0F) > 0x0F);
    GB_SET_F(GB_C_FLAG, sum > 0xFF);
    
    //Used to set F reg 
    GB_F_OR_AF(ctx, tmpRegF);
}

uint8_t GB_AND_R(EmulationState *ctx)
{
    // encoding: 0b10100xxx
    /*
    result = A & B
    A = result
    flags.Z = 1 if result == 0 else 0
    */
    const uint8_t r = (ctx->registers->INSTRUCTION & 0x07);
    uint8_t and = ctx->registers->A & GB_GetReg8(ctx, r);
    ctx->registers->A = and;

    GB_TMP_F();
    GB_SET_F(GB_ZERO_FLAG, and == 0);

    //Used to set F reg 
    GB_F_OR_AF(ctx, tmpRegF);
}

uint8_t GB_AND_N(EmulationState *ctx)
{
    // encoding: 0b11100110 
    /*
    n = read(PC++)
    result = A & n
    A = result
    flags.Z = 1 if result == 0 else 0
    flags.N = 0
    flags.H = 1
    flags.C = 0
    */
    const uint8_t n = GB_BusRead(ctx, ctx->registers->PC++);
    uint8_t and = ctx->registers->A & n;
    ctx->registers->A = and;

    GB_TMP_F();
    GB_SET_F(GB_ZERO_FLAG, and == 0);
    GB_SET_F(GB_N_FLAG, 0);
    GB_SET_F(GB_H_FLAG, 1);
    GB_SET_F(GB_C_FLAG, 0);

    //Used to set F reg 
    GB_F_OR_AF(ctx, tmpRegF);
}

uint8_t GB_AND_HL(EmulationState *ctx)
{
    // encoding: 0b10100110
    /*
    data = read(HL)
    result = A & data
    A = result
    flags.Z = 1 if result == 0 else 0
    flags.N = 0
    flags.H = 1
    flags.C = 0
    */
    const uint8_t data = GB_BusRead(ctx, ctx->registers->HL);
    uint8_t and = ctx->registers->A & data;
    ctx->registers->A = and;

    GB_TMP_F();
    GB_SET_F(GB_ZERO_FLAG, and == 0);
    GB_SET_F(GB_N_FLAG, 0);
    GB_SET_F(GB_H_FLAG, 1);
    GB_SET_F(GB_C_FLAG, 0);
    
    //Used to set F reg 
    GB_F_OR_AF(ctx, tmpRegF);
}

uint8_t GB_XOR_R(EmulationState *ctx)
{
    // encoding: 0b10101xxx
    /*
    # example: XOR B
    if opcode == 0xB8:
    result = A ^ B
    A = result
    flags.Z = 1 if result == 0 else 0
    flags.N = 0
    flags.H = 0
    flags.C = 0
    */
    const uint8_t r = (ctx->registers->INSTRUCTION & 0x07);
    uint8_t xor = ctx->registers->A ^ GB_GetReg8(ctx, r);
    GB_SetReg8(ctx, GB_A_OFFSET, xor);

    GB_TMP_F();
    GB_SET_F(GB_ZERO_FLAG, xor == 0);
    GB_SET_F(GB_N_FLAG, 0);
    GB_SET_F(GB_H_FLAG, 0);
    GB_SET_F(GB_C_FLAG, 0);

    //Used to set F reg 
    GB_F_OR_AF(ctx, tmpRegF);
}

uint8_t GB_XOR_N(EmulationState *ctx)
{
    // encoding: 0b11101110
    /*
        n = read(PC++)
        result = A ^ n
        A = result
        flags.Z = 1 if result == 0 else 0
        flags.N = 0
        flags.H = 0
        flags.C = 0
    */
    const uint8_t n = GB_BusRead(ctx, ctx->registers->PC++);
    uint8_t xor = ctx->registers->A ^ n;
    GB_SetReg8(ctx, GB_A_OFFSET, xor);

    GB_TMP_F();
    GB_SET_F(GB_ZERO_FLAG, xor == 0);
    GB_SET_F(GB_N_FLAG, 0);
    GB_SET_F(GB_H_FLAG, 0);
    GB_SET_F(GB_C_FLAG, 0);

    //Used to set F reg 
    GB_F_OR_AF(ctx, tmpRegF);
}

uint8_t GB_XOR_HL(EmulationState *ctx)
{
    // encoding: 0b10101110
    /*
        data = read(HL)
        result = A ^ data
        A = result
        flags.Z = 1 if result == 0 else 0
        flags.N = 0
        flags.H = 0
        flags.C = 0
    */

    const uint8_t data = GB_BusRead(ctx, ctx->registers->HL);
    uint8_t xor = ctx->registers->A ^ data;
    GB_SetReg8(ctx, GB_A_OFFSET, xor);

    GB_TMP_F();
    GB_SET_F(GB_ZERO_FLAG, xor == 0);
    GB_SET_F(GB_N_FLAG, 0);
    GB_SET_F(GB_H_FLAG, 1);
    GB_SET_F(GB_C_FLAG, 0);
    
    //Used to set F reg 
    GB_F_OR_AF(ctx, tmpRegF);
}
uint8_t GB_OR_R(EmulationState *ctx)
{
    // encoding: 0b10110xxx
    /*
        result = A | B
        A = result
        flags.Z = 1 if result == 0 else 0
        flags.N = 0
        flags.H = 0
        flags.C = 0
    */

    const uint8_t r = (ctx->registers->INSTRUCTION & 0x07);
    uint8_t or = ctx->registers->A | GB_GetReg8(ctx, r);
    GB_SetReg8(ctx, GB_A_OFFSET, or);

    GB_TMP_F();
    GB_SET_F(GB_ZERO_FLAG, or == 0);
    GB_SET_F(GB_N_FLAG, 0);
    GB_SET_F(GB_H_FLAG, 0);
    GB_SET_F(GB_C_FLAG, 0);

    //Used to set F reg 
    GB_F_OR_AF(ctx, tmpRegF);
}
uint8_t GB_OR_N(EmulationState *ctx)
{
    // encoding: 0b11110110
    /*
        n = read(PC++)
        result = A | n
        A = result
        flags.Z = 1 if result == 0 else 0
        flags.N = 0
        flags.H = 0
        flags.C = 0
    */
    const uint8_t n = GB_BusRead(ctx, ctx->registers->PC++);
    uint8_t or = ctx->registers->A | n;
    GB_SetReg8(ctx, GB_A_OFFSET, or);

    GB_TMP_F();
    GB_SET_F(GB_ZERO_FLAG, or == 0);
    GB_SET_F(GB_N_FLAG, 0);
    GB_SET_F(GB_H_FLAG, 0);
    GB_SET_F(GB_C_FLAG, 0);

    //Used to set F reg 
    GB_F_OR_AF(ctx, tmpRegF);
}
uint8_t GB_OR_HL(EmulationState *ctx)
{
    // encoding: 0b10110110
    /*
        data = read(HL)
        result = A | data
        A = result
        flags.Z = 1 if result == 0 else 0
        flags.N = 0
        flags.H = 0
        flags.C = 0
    */
    const uint8_t data = GB_BusRead(ctx, ctx->registers->HL);
    uint8_t or = ctx->registers->A | data;
    GB_SetReg8(ctx, GB_A_OFFSET, or);

    GB_TMP_F();
    GB_SET_F(GB_ZERO_FLAG, or == 0);
    GB_SET_F(GB_N_FLAG, 0);
    GB_SET_F(GB_H_FLAG, 0);
    GB_SET_F(GB_C_FLAG, 0);
    
    //Used to set F reg 
    GB_F_OR_AF(ctx, tmpRegF);
}

uint8_t GB_CP_R(EmulationState *ctx)
{
    // encoding: 0b10111xxx
    /*
        result, carry_per_bit = A - B
        flags.Z = 1 if result == 0 else 0
        flags.N = 1
        flags.H = 1 if carry_per_bit[3] else 0
        flags.C = 1 if carry_per_bit[7] else 0
    */
    const uint8_t r = (ctx->registers->INSTRUCTION & 0x07);
    const uint8_t sub =  ctx->registers->A - GB_GetReg8(ctx, r);

    GB_TMP_F();
    GB_SET_F(GB_ZERO_FLAG, sub == 0);
    GB_SET_F(GB_N_FLAG, 1);
    GB_SET_F(GB_H_FLAG, sub >> 3 == 0x01);
    GB_SET_F(GB_C_FLAG, sub >> 7 == 0x01);
    
    //Used to set F reg 
    GB_F_OR_AF(ctx, tmpRegF);
}

uint8_t GB_CP_N(EmulationState *ctx)
{
    // encoding: 0b11111110
    /*
        n = read(PC++)
        result, carry_per_bit = A - n
        flags.Z = 1 if result == 0 else 0
        flags.N = 1
        flags.H = 1 if carry_per_bit[3] else 0
        flags.C = 1 if carry_per_bit[7] else 0
    */

    const uint8_t n = GB_BusRead(ctx, ctx->registers->PC++);
    const uint8_t sub =  ctx->registers->A - n;

    GB_TMP_F();
    GB_SET_F(GB_ZERO_FLAG, sub == 0);
    GB_SET_F(GB_N_FLAG, 1);
    GB_SET_F(GB_H_FLAG, sub >> 3 == 0x01);
    GB_SET_F(GB_C_FLAG, sub >> 7 == 0x01);
    
    //Used to set F reg 
    GB_F_OR_AF(ctx, tmpRegF);
}

uint8_t GB_CP_HL(EmulationState *ctx)
{
    // encoding: 0b10111110
    /*
        data = read(HL)
        result, carry_per_bit = A - data
        flags.Z = 1 if result == 0 else 0
        flags.N = 1
        flags.H = 1 if carry_per_bit[3] else 0
        flags.C = 1 if carry_per_bit[7] else 0
    */
    const uint8_t data = GB_BusRead(ctx, ctx->registers->HL);
    const uint8_t sub =  ctx->registers->A - data;

    GB_TMP_F();
    GB_SET_F(GB_ZERO_FLAG, sub == 0);
    GB_SET_F(GB_N_FLAG, 1);
    GB_SET_F(GB_H_FLAG, sub >> 3 == 0x01);
    GB_SET_F(GB_C_FLAG, sub >> 7 == 0x01);
    
    //Used to set F reg 
    GB_F_OR_AF(ctx, tmpRegF);
}

uint8_t GB_INC_R(EmulationState *ctx)
{
    // encoding:0b00xxx100
    /*
        result, carry_per_bit = B + 1
        B = result
        flags.Z = 1 if result == 0 else 0
        flags.N = 0
        flags.H = 1 if carry_per_bit[3] else 0
    */
    uint8_t result = 0;
    const uint8_t r = (ctx->registers->INSTRUCTION & 0x38) >> 3;
    result = GB_GetReg8(ctx, r);
    GB_SetReg8(ctx, r, ++result);

    GB_TMP_F();
    GB_SET_F(GB_ZERO_FLAG, result == 0);
    GB_SET_F(GB_N_FLAG, 0);
    GB_SET_F(GB_H_FLAG, result >> 3 == 0x01);
    
    //Used to set F reg 
    GB_F_OR_AF(ctx, tmpRegF);
}

uint8_t GB_INC_HL(EmulationState *ctx)
{
    // encoding: 0b00110100
    /*
        data = read(HL)
        result, carry_per_bit = data + 1
        write(HL, result)
        flags.Z = 1 if result == 0 else 0
        flags.N = 0
        flags.H = 1 if carry_per_bit[3] else 0
    */
    uint8_t result = 0;
    const uint8_t data = GB_BusRead(ctx, ctx->registers->HL);
    
    result = data + 1; 
    GB_BusWrite(ctx, ctx->registers->HL, result);

    GB_TMP_F();
    GB_SET_F(GB_ZERO_FLAG, result == 0);
    GB_SET_F(GB_N_FLAG, 0);
    GB_SET_F(GB_H_FLAG, result >> 3 == 0x01);
    
    //Used to set F reg 
    GB_F_OR_AF(ctx, tmpRegF);
}

uint8_t GB_DEC_R(EmulationState *ctx)
{
    // encoding: 0b00xxx101
    /*
        result, carry_per_bit = B - 1
        B = result
        flags.Z = 1 if result == 0 else 0
        flags.N = 1
        flags.H = 1 if carry_per_bit[3] else 0
    */
    uint8_t result = 0;
    const uint8_t r = (ctx->registers->INSTRUCTION & 0x38) >> 3;
    result = GB_GetReg8(ctx, r);
    GB_SetReg8(ctx, r, --result);

    GB_TMP_F();
    GB_SET_F(GB_ZERO_FLAG, result == 0);
    GB_SET_F(GB_N_FLAG, 1);
    GB_SET_F(GB_H_FLAG, result >> 3 == 0x01);
    
    //Used to set F reg 
    GB_F_OR_AF(ctx, tmpRegF);
}

uint8_t GB_DEC_HL(EmulationState *ctx)
{
    // encoding: 0b00110101
    /*
        data = read(HL)
        result, carry_per_bit = data - 1
        write(HL, result)
        flags.Z = 1 if result == 0 else 0
        flags.N = 1
        flags.H = 1 if carry_per_bit[3] else 0
    */
    uint8_t result = 0;
    const uint8_t data = GB_BusRead(ctx, ctx->registers->HL);
    
    result = data - 1; 
    GB_BusWrite(ctx, ctx->registers->HL, result);

    GB_TMP_F();
    GB_SET_F(GB_ZERO_FLAG, result == 0);
    GB_SET_F(GB_N_FLAG, 1);
    GB_SET_F(GB_H_FLAG, result >> 3 == 0x01);
    
    //Used to set F reg 
    GB_F_OR_AF(ctx, tmpRegF);
}

uint8_t GB_DAA(EmulationState *ctx)
{
    // encoding: 0b00100111
    /*
      just flags???
    */
    
}

uint8_t GB_CPL(EmulationState *ctx)
{
    // encoding:0b00101111
    /*
    A = ~A
    flags.N = 1
    flags.H = 1
    */
    uint8_t A = ctx->registers->A;
    GB_SetReg8(ctx, GB_A_OFFSET, ~A);

    GB_TMP_F();
    GB_SET_F(GB_N_FLAG, 1);
    GB_SET_F(GB_H_FLAG, 1);
    
    //Used to set F reg 
    GB_F_OR_AF(ctx, tmpRegF);
}

// 16 BIT ALU INSTRUCTIONS
uint8_t GB_ADD_HL_RR(EmulationState *ctx)
{
    // encoding: 0b00xx1001/various 
    /*
        HL = HL+rr     ; rr may be BC,DE,HL,SP
    */
       /*
    HL = HL + rr; rr may be BC, DE, HL, SP
    Flags affected:
    Z - Not affected
    N - 0
    H - Set if carry from bits 11-12
    C - Set if carry from bits 15-16
    */

    const uint8_t rr = (ctx->registers->INSTRUCTION & 0x30) >> 4; // Extract rr from instruction
    const uint16_t rr_value = GB_GetReg16(ctx, rr, REG16_MODE_SP);

    // Calculate the result
    uint32_t result = ctx->registers->HL + rr_value;

    // Update HL with the result
    GB_SetReg16(ctx, GB_HL_OFFSET, (uint16_t)result, REG16_MODE_SP);
    ctx->registers->HL = result & 0xFFFF;
    
    // Set the flags
    GB_TMP_F();
    GB_SET_F(GB_ZERO_FLAG, 0);
    GB_SET_F(GB_N_FLAG, 0);
    GB_SET_F(GB_H_FLAG, ((ctx->registers->HL & 0xFFF) + (rr_value & 0xFFF)) > 0xFFF);
    GB_SET_F(GB_C_FLAG, result > 0xFFFF);

    return 1; // TODO: CHECK  TIMING...
}

uint8_t GB_INC_RR(EmulationState *ctx)
{
    // encoding: 0b00xx0011
    /*
        rr = rr+1      ; rr may be BC,DE,HL,SP
    */
    const uint8_t rr = (ctx->registers->INSTRUCTION & 0X30) >> 4; // Extract rr from instruction
    const uint16_t rrInc = GB_GetReg16(ctx, rr, REG16_MODE_SP) + 1;
    GB_SetReg16(ctx, rr, rrInc, REG16_MODE_SP);
    return 1; // TODO: CHECK  TIMING...
}

uint8_t GB_DEC_RR(EmulationState *ctx)
{
    // encoding: 0b00rr1011
    /*
        rr = rr-1      ; rr may be BC,DE,HL,SP
    */
    const uint8_t rr = (ctx->registers->INSTRUCTION & 0x30) >> 4; // Extract rr from instruction
    const uint16_t rrDec = GB_GetReg16(ctx, rr, REG16_MODE_SP) - 1;
    GB_SetReg16(ctx, rr, rrDec, REG16_MODE_SP);
    return 1; // TODO: CHECK  TIMING...
}

uint8_t GB_ADD_SP_DD(EmulationState *ctx)
{
    // encoding: E8 dd 
    /*
        SP = SP +/- dd ; dd is 8-bit signed number
    */
    const char dd = (char) GB_BusRead(ctx, ctx->registers->PC++);
    const short sum =  ctx->registers->SP + dd; // TODO: VERIFY CONVERSION
    ctx->registers->SP = sum;
    
    GB_TMP_F();
    GB_SET_F(GB_ZERO_FLAG,0);
    GB_SET_F(GB_N_FLAG, 0);
    GB_SET_F(GB_H_FLAG, (sum & 0x0F) > 0x0F);
    GB_SET_F(GB_C_FLAG, sum > 0xFF);
    
    //Used to set F reg 
    GB_F_OR_AF(ctx, tmpRegF);
    return 1; // TODO: CHECK  TIMING...
}

uint8_t GB_LD_HL_SP_PLUS_DD(EmulationState *ctx)
{
    // encoding: F8 dd
    /*
        HL = SP +/- dd ; dd is 8-bit signed number
    */

    const char dd = (char) GB_BusRead(ctx, ctx->registers->PC++);
    const short sum =  ctx->registers->SP + dd; //TODO: check conversion
    ctx->registers->HL = sum;

    GB_TMP_F();
    GB_SET_F(GB_ZERO_FLAG,0);
    GB_SET_F(GB_N_FLAG, 0);
    GB_SET_F(GB_H_FLAG, (sum & 0x0F) > 0x0F);
    GB_SET_F(GB_C_FLAG, sum > 0xFF);
    
    //Used to set F reg 
    GB_F_OR_AF(ctx, tmpRegF);
    return 1; // TODO: CHECK  TIMING...
}

// ROTATE AND SHIFT INSTRUCTIONS
uint8_t GB_RLCA(EmulationState *ctx)
{
    // encoding: 0x07
    /*
        rotate A left trough carry
    */   
   const uint8_t c = GB_TEST_F(ctx, GB_C_FLAG);
   uint8_t shifted = ctx->registers->A << 1 << c;
   ctx->registers->A = shifted;
   
    GB_TMP_F();
    GB_SET_F(GB_ZERO_FLAG, 0);
    GB_SET_F(GB_N_FLAG, 0);
    GB_SET_F(GB_H_FLAG, 0);
    GB_SET_F(GB_C_FLAG, shifted >> 7 == 0x01);
    
    //Used to set F reg 
    GB_F_OR_AF(ctx, tmpRegF);
    return 1; // TODO: CHECK  TIMING...
}

uint8_t GB_RLA(EmulationState *ctx)
{
    // encoding: 0x17
    /*
        rotate A left
    */

   uint8_t shifted = ctx->registers->A << 1;
   ctx->registers->A = shifted;
   
    GB_TMP_F();
    GB_SET_F(GB_ZERO_FLAG, 0);
    GB_SET_F(GB_N_FLAG, 0);
    GB_SET_F(GB_H_FLAG, 0);
    GB_SET_F(GB_C_FLAG, shifted >> 7 == 0x01);
    
    //Used to set F reg 
    GB_F_OR_AF(ctx, tmpRegF);
    return 1; // TODO: CHECK  TIMING...
}

uint8_t GB_RRCA(EmulationState *ctx)
{
    // encoding: 0x0F
    /*
        rotate right A through carry
    */
    const uint8_t c = GB_TEST_F(ctx, GB_C_FLAG);
    uint8_t shifted = ctx->registers->A >> 1 >> c;
    ctx->registers->A = shifted;

    GB_TMP_F();
    GB_SET_F(GB_ZERO_FLAG, 0);
    GB_SET_F(GB_N_FLAG, 0);
    GB_SET_F(GB_H_FLAG, 0);
    GB_SET_F(GB_C_FLAG, shifted >> 7 == 0x01);

    // Used to set F reg
    GB_F_OR_AF(ctx, tmpRegF);
    return 1; // TODO: CHECK  TIMING...
}

uint8_t GB_RRA(EmulationState *ctx)
{
    // encoding: 0x1F
    /*
        rotate A right
    */
    uint8_t shifted = ctx->registers->A >> 1;
    ctx->registers->A = shifted;

    GB_TMP_F();
    GB_SET_F(GB_ZERO_FLAG, 0);
    GB_SET_F(GB_N_FLAG, 0);
    GB_SET_F(GB_H_FLAG, 0);
    GB_SET_F(GB_C_FLAG, shifted >> 7 == 0x01);

    // Used to set F reg
    GB_F_OR_AF(ctx, tmpRegF);
    return 1; // TODO: CHECK  TIMING...
}

// CB PREFIX INSTRUCTIONS STARTS HERE!!!
// SINGLE BIT OPERATIONS (CB PREFIX)

uint8_t GB_RLC_R(EmulationState *ctx)
{
    // encoding: CB 0x 
    /*
        rotate left r trough carry
    */
   return 1; // TODO: CHECK  TIMING...
}

uint8_t GB_RLC_HL(EmulationState *ctx)
{
    //encoding: CB 06
    /*
        rotate left  hl trough carry
    */
   return 1; // TODO: CHECK  TIMING...
}

uint8_t GB_RL_R(EmulationState *ctx)
{
    // encoding: CB 1x
    /*
        rotate left through carry r
    */
   return 1; // TODO: CHECK  TIMING...
}

uint8_t GB_RL_HL(EmulationState *ctx)
{
    // encoding: CB 16 
    /*
        rotate left through carry hl
    */
   return 1; // TODO: CHECK  TIMING...
}

uint8_t GB_RRC_R(EmulationState *ctx)
{
    // encoding: CB 0x
    /*
        rotate right r
    */
   return 1; // TODO: CHECK  TIMING...
}

uint8_t GB_RRC_HL(EmulationState *ctx)
{
    // encoding: CB 0E
    /*
         rotate right (hl)
    */
   return 1; // TODO: CHECK  TIMING...
}

uint8_t GB_RR_R(EmulationState *ctx)
{
    // encoding: CB 1x
    /*
        rotate right through carry R
    */
   return 1; // TODO: CHECK  TIMING...
}

uint8_t GB_RR_HL(EmulationState *ctx)
{
    // encoding: CB 1E 
    /*
        rotate right through carry (hl)
    */
   return 1; // TODO: CHECK  TIMING...
}

uint8_t GB_SLA_R(EmulationState *ctx)
{
    // encoding: CB 2x
    /*
        shift left arithmetic (b0=0) r
    */
   return 1; // TODO: CHECK  TIMING...
}

uint8_t GB_SLA_HL(EmulationState *ctx)
{
    // encoding: CB 26
    /*
        shift left arithmetic (b0=0) (hl)
    */
   return 1; // TODO: CHECK  TIMING...
}

uint8_t GB_SWAP_R(EmulationState *ctx)
{
    // encoding: CB 3x
    /*
        exchange low/hi-nibble r
    */
   return 1; // TODO: CHECK  TIMING...
}

uint8_t GB_SWAP_HL(EmulationState *ctx)
{
    // encoding: CB 36
    /*

    */
   return 1; // TODO: CHECK  TIMING...
}

uint8_t GB_SRA_R(EmulationState *ctx)
{
    // encoding: CB 2x
    /*
        shift right arithmetic (b7=b7) r
    */
   return 1; // TODO: CHECK  TIMING...
}

uint8_t GB_SRA_HL(EmulationState *ctx)
{
    // encoding: CB 2E
    /*
        shift right arithmetic (b7=b7) (hl)
    */
   return 1; // TODO: CHECK  TIMING...
}
uint8_t GB_SRL_R(EmulationState *ctx)
{
    // encoding: CB 3x 
    /*
        shift right logical (b7=0) r
    */
   return 1; // TODO: CHECK  TIMING...
}
uint8_t GB_SRL_HL(EmulationState *ctx)
{
    // encoding: CB 3E 
    /*
        shift right logical (b7=0) (hl)
    */
   return 1; // TODO: CHECK  TIMING...
}

uint8_t GB_CB_BIT_N_R(EmulationState *ctx)
{
    // encoding: CB xx
    /*
        test bit n 
    */
   return 1; // TODO: CHECK  TIMING...
}   

uint8_t GB_CB_BIT_N_HL(EmulationState *ctx)
{
    // encoding: CB xx
    /*
        test bit n  
    */
   return 1; // TODO: CHECK  TIMING...
}

uint8_t GB_CB_SET_N_R(EmulationState *ctx)
{
    // encoding: CB xx
    /*
        set bit n
    */
   return 1; // TODO: CHECK  TIMING...
}   

uint8_t GB_CB_SET_N_HL(EmulationState *ctx)
{
    // encoding: CB xx
    /*
        reset bit n
    */
   return 1; // TODO: CHECK  TIMING...
}

uint8_t GB_CB_RES_N_R(EmulationState *ctx)
{
    // encoding: CB xx
    /*
        reset bit n
    */
   return 1; // TODO: CHECK  TIMING...
}   

uint8_t GB_CB_RES_N_HL(EmulationState *ctx)
{
    // encoding: CB xx
    /*
        reset bit n
    */
   return 1; // TODO: CHECK  TIMING...
}

// CPU CONTROL INSTRUCTIONS
uint8_t GB_CCF(EmulationState *ctx)
{
    // encoding: 3F
    /*
        flags.N = 0
        flags.H = 0
        flags.C = ~flags.C
    */
    const uint8_t c = GB_TEST_F(ctx, GB_C_FLAG);

    GB_TMP_F();
    GB_SET_F(GB_N_FLAG, 0);
    GB_SET_F(GB_H_FLAG, 0);
    GB_SET_F(GB_C_FLAG, c);

    // Used to set F reg
    GB_F_OR_AF(ctx, tmpRegF);

    return 1;
}

uint8_t GB_SCF(EmulationState *ctx)
{
    // encoding: 37
    /*
        flags.N = 0
        flags.H = 0
        flags.C = 1
    */
    GB_TMP_F();
    GB_SET_F(GB_N_FLAG, 0);
    GB_SET_F(GB_H_FLAG, 0);
    GB_SET_F(GB_C_FLAG, 1);

    // Used to set F reg
    GB_F_OR_AF(ctx, tmpRegF);

   return 1;
}

uint8_t GB_NOP(EmulationState *ctx)
{
    // encoding: 00
    /*
        no operation
    */
   return 4;
}

uint8_t GB_HALT(EmulationState *ctx)
{
    // encoding: 76
    /*
        halt until interrupt occurs (low power)
    */
   
   return 0;// RETURNING 0 HALTS THE EMULATOR CLOCK(0 CLOCK CYCLES)...
}

uint8_t GB_STOP(EmulationState *ctx)
{
    // encoding: 10 00
    /*
        low power standby mode (VERY low power)
    */

   return 0; // stop the emulation by returning 0
}

uint8_t GB_DI(EmulationState *ctx)
{
    // encoding: F3
    /*
        disable interrupts, IME=0  
    */
   ctx->ime = 0x00;
   
   return 1;
}

uint8_t GB_EI(EmulationState *ctx)
{
    // encoding: FB
    /*
        enable interrupts, IME=1 
    */
   ctx->ime = 0x01;

   return 1;
}

// JUMP INSTRUCTIONS
uint8_t GB_JP_NN(EmulationState *ctx)
{
    // encoding: C3 nn nn
    /*
        jump to nn, PC=nn
    */
   return 1; // TODO: CHECK  TIMING...
}

uint8_t GB_JP_HL(EmulationState *ctx)
{
    // encoding: E9
    /*
        jump to HL, PC=HL 
    */
   return 1; // TODO: CHECK  TIMING...
}

uint8_t GB_JP_F_NN(EmulationState *ctx)
{
    // encoding: xx nn nn
    /*
        conditional jump if nz,z,nc,c
    */
   return 1; // TODO: CHECK  TIMING...
}

uint8_t GB_JR_PC_PLUS_DD(EmulationState *ctx)
{
    // encoding: 18 dd
    /*
        relative jump to nn (PC=PC+8-bit signed)
    */
   return 1; // TODO: CHECK  TIMING...
}

uint8_t GB_JR_F_PC_PLUS_DD(EmulationState *ctx)
{
    // encoding: xx dd
    /*
        conditional relative jump if nz,z,nc,c
    */
   return 1; // TODO: CHECK  TIMING...
}

uint8_t GB_CALL_NN(EmulationState *ctx)
{
    // encoding: CD nn nn 
    /*
        call to nn, SP=SP-2, (SP)=PC, PC=nn
    */
   return 1; // TODO: CHECK  TIMING...
}
uint8_t GB_CALL_F_NN(EmulationState *ctx)
{
    // encoding: xx nn nn
    /*
        conditional call if nz,z,nc,c
    */
   return 1; // TODO: CHECK  TIMING...
}
uint8_t GB_RET(EmulationState *ctx)
{
    // encoding: C9
    /*
        return, PC=(SP), SP=SP+2  
    */
   return 1; // TODO: CHECK  TIMING...
}
uint8_t GB_RET_F(EmulationState *ctx)
{
    // encoding: xx
    /*
        conditional return if nz,z,nc,c
    */
   return 1; // TODO: CHECK  TIMING...
}

uint8_t GB_RETI(EmulationState *ctx)
{
    // encoding: D9
    /*
        return and enable interrupts (IME=1)
    */
   return 1; // TODO: CHECK  TIMING...
}   

uint8_t GB_RST_N(EmulationState *ctx)
{
    // encoding: xx
    /*
        call to 00,08,10,18,20,28,30,38
    */
   return 1; // TODO: CHECK  TIMING...
}

uint8_t GB_CB_PREFIX_INSTRUCTIONS(EmulationState *ctx)
{
    //INSERT HERE THE DECODING PROCESSES OF CB PREFIX INSTRUCTIONS...
}
