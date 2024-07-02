#include <SOC/GB_CPU.h>
#include <SOC/GB_Bus.h>
#include <SOC/GB_Registers.h>
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
    return 1; // TODO, PLACEHOLDER TIMING...;
}

uint8_t GB_LD_A_BC(EmulationState *ctx)
{
    // encoding: 0b00001010
    /*
        A = read(BC)
    */
    ctx->registers->A = GB_BusRead(ctx, ctx->registers->BC);
    return 1; // TODO, PLACEHOLDER TIMING...;
}

uint8_t GB_LD_A_DE(EmulationState *ctx)
{
    // encoding: 0b00011010
    /*
        A = read(DE)
    */
    ctx->registers->A = GB_BusRead(ctx, ctx->registers->DE);
    return 1; // TODO, PLACEHOLDER TIMING...;
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

    ctx->registers->A = GB_BusRead(ctx, nn);

    return 1; // TODO, PLACEHOLDER TIMING...;
}

uint8_t GB_LD_BC_A(EmulationState *ctx)
{
    //encoding: 0b00000010
    /*
        write(BC, A)
    */
    GB_BusWrite(ctx, ctx->registers->BC, ctx->registers->A);
    return 1; // TODO, PLACEHOLDER TIMING...
}

uint8_t GB_LD_DE_A(EmulationState *ctx)
{
    //encoding: 0b00010010
    /*
        write(DE, A)
    */
    GB_BusWrite(ctx, ctx->registers->DE, ctx->registers->A);

    return 1; // TODO, PLACEHOLDER TIMING...;
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

    return 1; // TODO, PLACEHOLDER TIMING...;
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

    ctx->registers->A = GB_BusRead(ctx, u16);

    return 1; // TODO, PLACEHOLDER TIMING...;
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
    return 1; // TODO, PLACEHOLDER TIMING...
}

uint8_t GB_LDH_A_C(EmulationState *ctx)
{
    // encoding: 0b11110010
    /*
        A = read(unsigned_16(lsb=C, msb=0xFF))
    */
    const uint16_t u16 = (uint16_t)(ctx->registers->C | (0xFF << 8));

    ctx->registers->A = GB_BusRead(ctx, u16);
    return 1; // TODO, PLACEHOLDER TIMING...
}


uint8_t GB_LDH_C_A(EmulationState *ctx)
{
    // encoding: 0b11100010
    /*
        write(unsigned_16(lsb=C, msb=0xFF), A)
    */
    const uint16_t u16 = (uint16_t)(ctx->registers->C | (0xFF << 8));
    GB_BusWrite(ctx, u16, ctx->registers->A);

    return 1; // TODO, PLACEHOLDER TIMING...
}

uint8_t GB_LDI_HL_A(EmulationState *ctx)
{
    // encoding: 0b00100010
    /*
        write(HL++, A)
    */
    GB_BusWrite(ctx, ctx->registers->HL++, ctx->registers->A);
    return 1; // TODO, PLACEHOLDER TIMING...
}

uint8_t GB_LDI_A_HL(EmulationState *ctx)
{
    // encoding: 0b00101010
    /*
        A = read(HL++)
    */

    ctx->registers->A = GB_BusRead(ctx, ctx->registers->HL++);
    return 1; // TODO, PLACEHOLDER TIMING...
}

uint8_t GB_LDD_HL_A(EmulationState *ctx)
{
    // encoding: 0b00110010
    /*
        write(HL--, A)
    */

    GB_BusWrite(ctx, ctx->registers->HL--, ctx->registers->A);
    return 1; // TODO, PLACEHOLDER TIMING...
}

uint8_t GB_LDD_A_HL(EmulationState *ctx)
{
    // encoding: 0b00111010
    /*
        A = read(HL--)
    */

    ctx->registers->A = GB_BusRead(ctx, ctx->registers->HL--);
    return 1; // TODO, PLACEHOLDER TIMING...
}

// 16 BIT LOAD INSTRUCTIONS
uint8_t GB_LD_RR_NN(EmulationState *ctx)
{
    //encoding: 0b00xx0001, xx : 0x30
    /*
        nn = unsigned_16(lsb=read(PC++), msb=read(PC++))
        rr = nn
    */
    const uint8_t rr = (ctx->registers->INSTRUCTION & 0x30) >> 4;
    const uint8_t lsb = GB_BusRead(ctx, ctx->registers->PC++);
    const uint8_t msb = GB_BusRead(ctx, ctx->registers->PC++);
    const uint16_t nn = (uint16_t)(lsb | (msb << 8));
    
    GB_SetReg16(ctx, rr, nn, REG16_MODE_SP);
    
    return 2;
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
    GB_SET_F(GB_H_FLAG, (sum  > 0x0F) );
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
    GB_SET_F(GB_H_FLAG, (sum  > 0x0F));
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
    GB_SET_F(GB_H_FLAG, (sum  > 0x0F));
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
    GB_SET_F(GB_H_FLAG, (sum  > 0x0F));
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
    GB_SET_F(GB_H_FLAG, (sum  > 0x0F));
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
    GB_SET_F(GB_H_FLAG, (sum  > 0x0F));
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
    GB_SET_F(GB_H_FLAG, (sum  > 0x0F));
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
    GB_SET_F(GB_H_FLAG, (sum  > 0x0F));
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
    GB_SET_F(GB_H_FLAG, (sum  > 0x0F));
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
    GB_SET_F(GB_H_FLAG, (sum  > 0x0F));
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
    GB_SET_F(GB_H_FLAG, (sum  > 0x0F));
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
    GB_SET_F(GB_H_FLAG, (sum  > 0x0F));
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
    ctx->registers->A = xor;

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
    ctx->registers->A = xor;

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
    ctx->registers->A = xor;

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
    ctx->registers->A = or;

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
    ctx->registers->A = or;

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
    ctx->registers->A = or;

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
    return 1; // TODO: CHECK  TIMING...
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
    return 1; // TODO: CHECK  TIMING...
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
    return 1; // TODO: CHECK  TIMING...
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
    return 1; // TODO: CHECK  TIMING...

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
    return 1; // TODO: CHECK  TIMING...
}

uint8_t GB_DAA(EmulationState *ctx)
{
    // encoding: 0b00100111
    /*
      just flags???
    */
    return 1; // TODO: CHECK  TIMING...
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
    ctx->registers->A = ~A;

    GB_TMP_F();
    GB_SET_F(GB_N_FLAG, 1);
    GB_SET_F(GB_H_FLAG, 1);
    
    //Used to set F reg 
    GB_F_OR_AF(ctx, tmpRegF);
    return 1; // TODO: CHECK  TIMING...
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
    GB_SET_F(GB_H_FLAG, (sum  > 0x0F));
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
    GB_SET_F(GB_H_FLAG, (sum  > 0x0F));
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
    MNE_Log("[GB_RLC_R]\n");

   return 1; // TODO: CHECK  TIMING...
}


uint8_t GB_RL_R(EmulationState *ctx)
{
    // encoding: CB 1x
    /*
        rotate left through carry r
    */
    MNE_Log("[GB_RL_R]\n");

   return 1; // TODO: CHECK  TIMING...
}

uint8_t GB_RRC_R(EmulationState *ctx)
{
    // encoding: CB 0x
    /*
        rotate right r
    */
    MNE_Log("[GB_RRC_R]\n");

   return 1; // TODO: CHECK  TIMING...
}



uint8_t GB_RR_R(EmulationState *ctx)
{
    // encoding: CB 1x
    /*
        rotate right through carry R
    */
    MNE_Log("[GB_RR_R]\n");

   return 1; // TODO: CHECK  TIMING...
}



uint8_t GB_SLA_R(EmulationState *ctx)
{
    // encoding: CB 2x
    /*
        shift left arithmetic (b0=0) r
    */
    MNE_Log("[GB_SLA_R]\n");

   return 1; // TODO: CHECK  TIMING...
}


uint8_t GB_SWAP_R(EmulationState *ctx)
{
    // encoding: CB 3x
    /*
        exchange low/hi-nibble r
    */
    MNE_Log("[GB_SWAP_R]\n");

   return 1; // TODO: CHECK  TIMING...
}


uint8_t GB_SRA_R(EmulationState *ctx)
{
    // encoding: CB 2x
    /*
        shift right arithmetic (b7=b7) r
    */
    MNE_Log("[GB_SRA_R]\n");

   return 1; // TODO: CHECK  TIMING...
}


uint8_t GB_SRL_R(EmulationState *ctx)
{
    // encoding: CB 3x 
    /*
        shift right logical (b7=0) r
    */
    MNE_Log("[GB_SRL_R]\n");

   return 1; // TODO: CHECK  TIMING...
}


uint8_t GB_CB_BIT_N_R(EmulationState *ctx)
{
    /*
        test bit n of r
    */
    // encoding: 0xCB 0x40
    MNE_Log("[GB_CB_BIT_N_R]\n");
    uint8_t r8 = ctx->registers->INSTRUCTION & 0x07;
    uint8_t b3 = ctx->registers->INSTRUCTION & 0x38 >> 3;
    uint8_t bitTest = ((GB_GetReg8(ctx, r8) >> b3) & 0x01) == 0x00;

    GB_TMP_F();
    GB_SET_F(GB_N_FLAG, 0);
    GB_SET_F(GB_H_FLAG,  bitTest > 0x0F);
    GB_SET_F(GB_ZERO_FLAG, bitTest);
    GB_F_OR_AF(ctx, tmpRegF);

   return 1; // TODO: CHECK  TIMING...
}   

uint8_t GB_CB_RES_N_R(EmulationState *ctx)
{
    // encoding: CB xx
    /*
        reset bit n of r
    */
    MNE_Log("[GB_CB_RES_N_R]\n");

    uint8_t r8 = ctx->registers->INSTRUCTION & 0x07;
    uint8_t b3 = ctx->registers->INSTRUCTION & 0x38 >> 3;

    uint8_t clearBit = ((GB_GetReg8(ctx, r8)) & ~(1 << b3));
    GB_SetReg8(ctx, r8, clearBit);

    return 1; // TODO: CHECK  TIMING...
}

uint8_t GB_CB_SET_N_R(EmulationState *ctx)
{
    // encoding: CB xx
    /*
        set bit n
    */
    MNE_Log("[GB_CB_SET_N_R]\n");

    uint8_t r8 = ctx->registers->INSTRUCTION & 0x07;
    uint8_t b3 = ctx->registers->INSTRUCTION & 0x38 >> 3;

    uint8_t setBit = ((GB_GetReg8(ctx, r8)) | (1 << b3));
    GB_SetReg8(ctx, r8, setBit);

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
   
   return 0; // stop the emulation by returning 0
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
    /*
    Opcode 0b11000011/0xC3 Duration 4 machine cycles
    Length 3 bytes: opcode + LSB(nn) + MSB(nn)

    nn_lsb = read_memory(addr=PC); PC = PC + 1
    nn_msb = read_memory(addr=PC); PC = PC + 1
    nn = unsigned_16(lsb=nn_lsb, msb=nn_msb)
    PC = nn
    */

    const uint8_t nn_lsb = GB_BusRead(ctx, ctx->registers->PC++);
    const uint8_t nn_msb = GB_BusRead(ctx, ctx->registers->PC++);
    ctx->registers->PC = (uint16_t)(nn_lsb | (nn_msb << 8));

   return 4;
}

uint8_t GB_JP_HL(EmulationState *ctx)
{
    /*
    Opcode 0b11101001/0xE9 Duration 1 machine cycle
    Length 1 byte: opcode
    
        PC = HL
    */

   ctx->registers->PC = ctx->registers->HL;
   
   return 1;
}

uint8_t GB_JP_CC_NN(EmulationState *ctx)
{
    /*
    Opcode 0b110xx010/various
    Duration 4 machine cycles (cc=true)
    3 machine cycles (cc=false)
    Length 3 bytes: opcode + LSB(nn) + MSB(nn)
    conditional jump if nz,z,nc,c

    nn_lsb = read_memory(addr=PC); PC = PC + 1
    nn_msb = read_memory(addr=PC); PC = PC + 1
    nn = unsigned_16(lsb=nn_lsb, msb=nn_msb)
    if !flags.Z: # cc=true
    PC = nn
    */

    const uint8_t cc = (ctx->registers->INSTRUCTION & 0x18) >> 3;

    if (GB_ResolveCondition(ctx, cc))
    {
        const uint8_t nn_lsb = GB_BusRead(ctx, ctx->registers->PC++);
        const uint8_t nn_msb = GB_BusRead(ctx, ctx->registers->PC++);
        ctx->registers->PC = (uint16_t)(nn_lsb | (nn_msb << 8));
        return 4;
    }

    return 3; // TODO: CHECK  TIMING...
}

uint8_t GB_JR_E(EmulationState *ctx)
{
    /*
    Opcode 0b00011000/0x18 Duration 3 machine cycles
    Length 2 bytes: opcode + e

    e = signed_8(read_memory(addr=PC)); PC = PC + 1
    PC = PC + e
    */
    const int8_t nn_signed = GB_BusRead(ctx, ctx->registers->PC++);
    ctx->registers->PC += nn_signed;

   return 3; 
}

uint8_t GB_JR_CC_E(EmulationState *ctx)
{
    /*
    Opcode 0b001xx000/various Duration
    3 machine cycles (cc=true)
    2 machine cycles (cc=false)
    Length 2 bytes: opcode + e
    */

    const uint8_t cc = (ctx->registers->INSTRUCTION & 0x18) >> 3;

    if (GB_ResolveCondition(ctx, cc))
    {
        const int8_t nn_signed = GB_BusRead(ctx, ctx->registers->PC++);
        ctx->registers->PC += nn_signed;
        return 3;
    }

    return 2; // TODO: CHECK  TIMING...
}

uint8_t GB_CALL_NN(EmulationState *ctx)
{
    /*
    Opcode 0b11001101/0xCD
    Duration 6 machine cycles
    Length 3 bytes: opcode + LSB(nn) + MSB(nn) Flags -

    nn_lsb = read_memory(addr=PC); PC = PC + 1
    nn_msb = read_memory(addr=PC); PC = PC + 1
    nn = unsigned_16(lsb=nn_lsb, msb=nn_msb)
    SP = SP - 1
    write_memory(addr=SP, data=msb(PC)); SP = SP - 1
    write_memory(addr=SP, data=lsb(PC))
    PC = nn
    */
    const uint8_t nn_lsb = GB_BusRead(ctx, ctx->registers->PC++);
    const uint8_t nn_msb = GB_BusRead(ctx, ctx->registers->PC++);
    const uint16_t addr = (uint16_t)(nn_lsb | (nn_msb << 8));
    ctx->registers->SP--;


    GB_BusWrite(ctx,ctx->registers->SP--, ctx->registers->PC >> 8);
    GB_BusWrite(ctx,ctx->registers->SP, ctx->registers->PC & 0xFF);
    ctx->registers->PC = addr;

    return 6;
}

uint8_t GB_CALL_CC_NN(EmulationState *ctx)
{
    /*
    Opcode 0b110xx100/various
    Duration 6 machine cycles (cc=true)
    3 machine cycles (cc=false)
    Length 3 bytes: opcode + LSB(nn) + MSB(nn) Flags -

    nn_lsb = read_memory(addr=PC); PC = PC + 1
    nn_msb = read_memory(addr=PC); PC = PC + 1
    nn = unsigned_16(lsb=nn_lsb, msb=nn_msb)

    if !flags.Z: # cc=true
        SP = SP - 1
        write_memory(addr=SP, data=msb(PC)); SP = SP - 1
        write_memory(addr=SP, data=lsb(PC))
        PC = nn

    */
    const uint8_t cc = (ctx->registers->INSTRUCTION & 0x18) >> 3;

    if (GB_ResolveCondition(ctx, cc))
    {
        const uint8_t nn_lsb = GB_BusRead(ctx, ctx->registers->PC++);
        const uint8_t nn_msb = GB_BusRead(ctx, ctx->registers->PC++);
        const uint16_t addr = (uint16_t)(nn_lsb | (nn_msb << 8));

        ctx->registers->SP--;

        GB_BusWrite(ctx, ctx->registers->SP--, ctx->registers->PC >> 8);
        GB_BusWrite(ctx, ctx->registers->SP, ctx->registers->PC & 0xFF);
        ctx->registers->PC = addr;
        return 6;
    }

    return 3;
}

uint8_t GB_RET(EmulationState *ctx)
{
    // encoding: C9
    /*
        lsb = read_memory(addr=SP); SP = SP + 1
        msb = read_memory(addr=SP); SP = SP + 1
        PC = unsigned_16(lsb=lsb, msb=msb)
    */
    const uint8_t lsb = GB_BusRead(ctx, ctx->registers->SP++);
    const uint8_t msb = GB_BusRead(ctx, ctx->registers->SP++);

    ctx->registers->PC = (uint16_t)(lsb | (msb << 8));
    return 4;
}

uint8_t GB_RET_CC(EmulationState *ctx)
{
    /*
    Opcode 0b110xx000/various
    Duration 5 machine cycles (cc=true)
    2 machine cycles (cc=false)

    if opcode == 0xC0: # example: RET NZ
        if !flags.Z: # cc=true
        lsb = read_memory(addr=SP); SP = SP + 1
        msb = read_memory(addr=SP); SP = SP + 1
        PC = unsigned_16(lsb=lsb, msb=msb)
    */
    const uint8_t cc = (ctx->registers->INSTRUCTION & 0x18) >> 3;

    if (GB_ResolveCondition(ctx, cc))
    {
        const uint8_t lsb = GB_BusRead(ctx, ctx->registers->SP++);
        const uint8_t msb = GB_BusRead(ctx, ctx->registers->SP++);

        ctx->registers->PC = (uint16_t)(lsb | (msb << 8));
        return 5;
    }

    return 2;
}

uint8_t GB_RETI(EmulationState *ctx)
{
    /*
        Opcode 0b11011001/0xD9
        Duration 4 machine cycles
        Length 1 byte: opcode

        lsb = read_memory(addr=SP); SP = SP + 1
        msb = read_memory(addr=SP); SP = SP + 1
        PC = unsigned_16(lsb=lsb, msb=msb)
        IME = 1
    */
    const uint8_t lsb = GB_BusRead(ctx, ctx->registers->SP++);
    const uint8_t msb = GB_BusRead(ctx, ctx->registers->SP++);
    ctx->registers->PC = (uint16_t)(lsb | (msb << 8));
    ctx->ime = 0x01;

    return 4;
}

uint8_t GB_RST_N(EmulationState *ctx)
{
    // TODO: IMPLEMENT INTERRUPTS!!!
    /*
       [call to 00,08,10,18,20,28,30,38 interrupt vectors]
        Opcode 0b11xxx111/various
        Duration 4 machine cycles
        Length 1 byte: opcode

        if opcode == 0xDF: # example: RST 0x18
            n = 0x18
            SP = SP - 1
            write_memory(addr=SP, data=msb(PC)); SP = SP - 1
            write_memory(addr=SP, data=lsb(PC))
            PC = unsigned_16(lsb=n, msb=0x00)
    */

    return 1; // TODO: CHECK  TIMING...
}

//TODO: MAKE MORE READABLE THIS FUNCTION AND AVOID MOVING CODE INTO FUNCTIONS (TO AVOID STACK OVERFLOWS ON MCU'S)
uint8_t GB_CB_PREFIX(EmulationState *ctx)
{
    const uint8_t cbInstr = ctx->memory[ctx->registers->PC++];
    MNE_Log("CB PREFIX OPCODE: [%02X] ", cbInstr);
    ctx->registers->INSTRUCTION = cbInstr;

    /*
        CB prefix instructions:
        there are two groups and they are grouped by mask (0xC0 and 0xF8)

        Group 0xC0
        Bit Position:
                     7  6       5  4  3             2  1  0
        bit b3, r8   0  1       Bit index (b3)      Operand (r8) | MASK: 0xC0, BASE-OP: 0x40
        res b3, r8   1  0       Bit index (b3)      Operand (r8) | MASK: 0xC0, BASE-OP: 0x80
        set b3, r8   1  1       Bit index (b3)      Operand (r8) | MASK: 0xC0, BASE-OP: 0xC0

        Group 0xF8
        Bit Position:
                     7  6  5  4  3      2  1  0
        rlc r8       0  0  0  0  0      Operand (r8) | MASK:0xF8, BASE-OP: 0X00
        rrc r8       0  0  0  0  1      Operand (r8) | MASK:0xF8, BASE-OP: 0x08
        rl r8        0  0  0  1  0      Operand (r8) | MASK:0xF8, BASE-OP: 0x10
        rr r8        0  0  0  1  1      Operand (r8) | MASK:0xF8, BASE-OP: 0x18
        sla r8       0  0  1  0  0      Operand (r8) | MASK:0xF8, BASE-OP: 0x20
        sra r8       0  0  1  0  1      Operand (r8) | MASK:0xF8, BASE-OP: 0x28
        swap r8      0  0  1  1  0      Operand (r8) | MASK:0xF8, BASE-OP: 0x30
        srl r8       0  0  1  1  1      Operand (r8) | MASK:0xF8, BASE-OP: 0x38

    */

    if ((cbInstr & 0xC0) != 0x00)
    {
        uint8_t masked = cbInstr & 0xC0;
        switch (masked)
        {
        case 0x40:
            GB_CB_BIT_N_R(ctx);
            break;
        case 0x80:
            GB_CB_RES_N_R(ctx);
            break;
        case 0xC0:
            GB_CB_SET_N_R(ctx);
            break;
        default:
            MNE_Log("[CB PREFIX SUB MASK 0xC8 BASE OP CODE NOT FOUND, MASKED VALUE [%04X]", masked);
            return 0;
            break;
        }
    }
    else
    {
        uint8_t masked = cbInstr & 0xF8;
        switch (masked)
        {
        case 0X00:
            GB_RLC_R(ctx);
            break;
        case 0x08:
            GB_RRC_R(ctx);
            break;
        case 0x10:
            GB_RL_R(ctx);
            break;
        case 0x18:
            GB_RR_R(ctx);
            break;
        case 0x20:
            GB_SLA_R(ctx);
            break;
        case 0x28:
            GB_SRA_R(ctx);
            break;
        case 0x30:
            GB_SWAP_R(ctx);
            break;
        case 0x38:
            GB_SRL_R(ctx);
            break;
        default:
            MNE_Log("[CB PREFIX SUB MASK 0xF8 BASE OP CODE NOT FOUND, MASKED VALUE [%04X]", masked);
            return 0;
        }
    }

    return 1;
}

uint8_t GB_ResolveCondition(const EmulationState *ctx, uint8_t cc)
{
    switch (cc)
    {
    case COND_NZ:
        return !GB_TEST_F(ctx, GB_ZERO_FLAG);
    case COND_Z:
        return GB_TEST_F(ctx, GB_ZERO_FLAG);
    case COND_NC:
        return !GB_TEST_F(ctx, GB_C_FLAG);
    case COND_C:
        return GB_TEST_F(ctx, GB_C_FLAG);

    default:
        MNE_Log("Cannot resolve CC condition(unknow value)");
    }
    
    return 0;
}