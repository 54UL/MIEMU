#include "CC8_Emulator.h"

#include <stdlib.h>
#include <stdio.h>
#define CC8_FONT_ADDR_START = 0x000;
#define CC8_BOOT_ADDR_START = 0x200; 
#define CC8_FILE_PROGRAM_BUFFER_SIZE 4096

static CC8_Machine *s_currentChipCtx = NULL;

// TODO:MOVE TO A PROPPER HEADER FILE called utils or such
void HexDump(uint8_t *buffer, size_t size)
{
    // Print header
    printf("Hex dump:\n");
    printf("---------------------------------------------------------------\n");
    printf("Offset |                         Hexadecimal                   \n");
    printf("-------|-------------------------------------------------------\n");

    // Print buffer contents in hexadecimal format with ASCII representation
    int i = 0;
    int j = 0;

    for (i = 0; i < size; i += 16)
    {
        // Print offset
        printf("%06X | ", i);

        // Print hexadecimal values
        for (j = i; j < i + 16 && j < size; j++)
        {
            printf("%02X ", buffer[j]);
        }
        printf("\n");
    }

    // Print footer
    printf("-----------------------------------------------------------------\n");
}

uint8_t CC8_LoadProgram(const char *filePath)
{
    if (s_currentChipCtx == NULL)
    {
        s_currentChipCtx = calloc(1, sizeof(CC8_Machine)); // Initialize machine state
    }

    FILE *file = fopen(filePath, "rb");
    if (file == NULL)
    {
        printf("Unable to open file %s\n", filePath);
        return 0;
    }

    // Get the size of the file
    fseek(file, 0L, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0L, SEEK_SET);

    // Allocate a buffer for reading the file
    uint8_t *buffer = (uint8_t *)calloc(1, file_size + 1);
    size_t bytes_read = fread(buffer, 1, file_size, file);
    s_currentChipCtx->PROGRAM_SIZE = bytes_read;
    s_currentChipCtx->PC = CC8_BOOT_ADDR_START;

    // LOAD PROGRAM
    uint16_t addr = 0;
    uint16_t loop_index = 0;
    for (addr = CC8_BOOT_ADDR_START; (addr < CC8_BOOT_ADDR_START + bytes_read); addr++)
    {
        s_currentChipCtx->RAM[addr] = buffer[loop_index++];
    }

    // LOAD FONT
    loop_index = 0;
    printf("Loaded font size: %i\n", sizeof(CC8_FONT));
    for (addr = CC8_FONT_ADDR_START; (addr < CC8_FONT_ADDR_START + sizeof(CC8_DEFAULT_FONT)); addr++)
    {
        s_currentChipCtx->RAM[CC8_FONT_ADDR_START + addr] = CC8_DEFAULT_FONT[loop_index++];
    }

    // TODO: ADD DEBUG FLAG for dumping hex
    HexDump(buffer, bytes_read);

    // Clean up resources
    free(buffer);
    fclose(file);
    return 1;
}

void CC8_QuitProgram()
{
    if (s_currentChipCtx != NULL)
    {
        free(s_currentChipCtx);
        s_currentChipCtx = NULL;
    }
}

void CC8_Step(uint16_t opcode)
{
    // Decode instruction
    uint8_t x = (opcode >> 8) & 0x0F;
    uint8_t y = (opcode >> 4) & 0x0F;
    uint16_t nnn = opcode & 0x0FFF;
    uint8_t kk = opcode & 0x00FF;
    uint8_t n = opcode & 0x000F;

    // Fetch and execute
    switch (opcode & 0xF000)
    {
    case 0x0000:
        switch (opcode & 0x00FF)
        {
        case 0x00E0:
            printf("CLS\n");
            CC8_CLS();
            break;

        case 0x00EE:
            printf("RET\n");
            CC8_RET();
            break;

        case 0x0000:
            break;

        default:
            printf("Unknow sub-instruction %06X\n", opcode & 0x00FF);
        }
        break;

    case 0x1000:
        printf("JMP 0x%03X\n", nnn);
        CC8_JMP(nnn);
        break;

    case 0x2000:
        printf("CALL 0x%03X\n", nnn);
        CC8_CALL(nnn);
        break;

    case 0x3000:
        printf("SE V%i, 0x%02X\n", x, kk);
        CC8_SE_VX_BYTE(x, kk);
        break;

    case 0x4000:
        printf("SNE V%i, 0x%02X\n", x, kk);
        CC8_SNE_VX_BYTE(x, kk);
        break;

    case 0x5000:
        printf("SE V%i, V%i\n", x, y);
        CC8_SE_VX_VY(x, y);
        break;

    case 0x6000:
        printf("LD_VX_BYTE V%i, %i\n", x, kk);
        CC8_LD_VX_BYTE(x, kk);
        break;

    case 0x7000:
        printf("ADD V%i, %i\n", x, kk);
        CC8_ADD_VX_BYTE(x, kk);
        break;

    case 0x8000:
        switch (opcode & 0x000F)
        {
        case 0x0000:
            printf("LD V%i, V%i\n", x, y);
            CC8_LD_VX_VY(x, y);
            break;

        case 0x0001:
            printf("OR V%i, V%i\n", x, y);
            CC8_OR_VX_VY(x, y);
            break;

        case 0x0002:
            printf("AND V%i, V%i\n", x, y);
            CC8_AND_VX_VY(x, y);
            break;

        case 0x0003:
            printf("XOR V%i, V%i\n", x, y);
            CC8_XOR_VX_VY(x, y);
            break;

        case 0x0004:
            printf("ADD V%i, V%i\n", x, y);
            CC8_ADD_VX_VY(x, y);
            break;

        case 0x0005:
            printf("SUB V%i, V%i\n", x, y);
            CC8_SUB_VX_VY(x, y);
            break;

        case 0x0006:
            printf("SHR V%i, V%i\n", x, y);
            CC8_SHR_VX_VY(x, y);
            break;

        case 0x0007:
            printf("SUBN V%i, V%i\n", x, y);
            CC8_SUBN_VX_VY(x, y);
            break;

        case 0x000E:
            printf("SHL V%i, V%i\n", x, y);
            CC8_SHL_VX_VY(x, y);
            break;

        default:

            printf("Unknow sub-instruction %06X\n", opcode & 0x000F);
        }
        break;

    case 0x9000:
        printf("SNE V%i, V%i\n", x, y);
        CC8_SNE_VX_VY(x, y);
        break;

    case 0xA000:
        printf("LD I, %i\n", nnn);
        CC8_LD_I_ADDR(nnn);
        break;

    case 0xB000:
        printf("JP V0, 0x%03X\n", nnn);
        CC8_JP_V0_ADDR(nnn);
        break;

    case 0xC000:
        printf("RND V%i, 0x%02X\n", x, kk);
        CC8_RND_VX_BYTE(x, kk);
        break;

    case 0xD000:
        printf("DRW v[%i], v[%i], %i\n", s_currentChipCtx->V[x], s_currentChipCtx->V[y], n);
        CC8_DRW_VX_VY_NIBBLE(x, y, n);
        break;

    case 0xE000:
        switch (opcode & 0x00FF)
        {
        case 0x9E:
            printf("SKP V%i\n", x);
            CC8_SKP_VX(x);
            break;

        case 0xA1:
            printf("SKNP V%i\n", x);
            CC8_SKNP_VX(x);
            break;

        default:
            printf("Unknow sub-instruction %06X\n", opcode & 0x00FF);
        }
        break;

    case 0xF000:
        switch (opcode & 0x00FF)
        {
        case 0x07:
            printf("LD V%i, DT\n", x);
            CC8_LD_VX_DT(x);
            break;

        case 0x0A:
            printf("LD V%i, K\n", x);
            CC8_LD_VX_K(x);
            break;

        case 0x15:
            printf("LD DT, V%i\n", x);
            CC8_LD_DT_VX(x);
            break;

        case 0x18:
            printf("LD ST, V%i\n", x);
            CC8_LD_ST_VX(x);
            break;

        case 0x1E:
            printf("ADD I, V%i\n", x);
            CC8_ADD_I_VX(x);
            break;

        case 0x29:
            printf("LD F, V%i\n", x);
            CC8_LD_F_VX(x);
            break;

        case 0x33:
            printf("LD B, V%i\n", x);
            CC8_LD_B_VX(x);
            break;

        case 0x55:
            printf("LD [I], V%i\n", x);
            CC8_LD_I_VX(x);
            break;

        case 0x65:
            printf("LD V%i, [I]\n", x);
            CC8_LD_VX_I(x);
            break;

        default:
            printf("Unknow sub-instruction %06X\n", opcode & 0x00FF);
        }
        break;

    default:
        printf("Unknow instruction opcode: %06X\n", opcode);
    }
}

void CC8_TickDelayTimer()
{
    if (s_currentChipCtx->DELAY != 0)
        s_currentChipCtx->DELAY--;
}

int CC8_TickEmulation()
{
    if (s_currentChipCtx == NULL) return 0;

    // CC8_DebugMachine(s_currentChipCtx, 1);
    // CC8_TickDelayTimer(); // TODO: FIX TIMING ISSUE (PUT THIS IN A SEPARATE THREAD)

    uint8_t higherByte = s_currentChipCtx->RAM[s_currentChipCtx->PC];
    uint8_t lowerByte = s_currentChipCtx->RAM[s_currentChipCtx->PC + 1];
    uint16_t value16 = (higherByte << 8) | lowerByte;

    CC8_Step(value16);

    s_currentChipCtx->PC += 2;
    return 1;
}

void CC8_SetKeyboardValue(uint8_t key)
{
    s_currentChipCtx->KEYBOARD = key;
}

void CC8_SetEmulationContext(CC8_Machine *context)
{
    s_currentChipCtx = context;
}

// ########## CHIP 8 INSTRUCTION SET IMPLEMENTATION
void CC8_CLS()
{
    for (uint16_t i = 0; i < sizeof(s_currentChipCtx->VRAM); i++)
    {
        s_currentChipCtx->VRAM[i] = 0xFF;
    }
}

void CC8_RET()
{
    s_currentChipCtx->PC = s_currentChipCtx->STACK[s_currentChipCtx->SP];
    s_currentChipCtx->SP--;
}

void CC8_JMP(uint16_t addr)
{
    s_currentChipCtx->PC = addr - 2;
}

void CC8_CALL(uint16_t addr)
{
    s_currentChipCtx->STACK[++s_currentChipCtx->SP] = s_currentChipCtx->PC;
    s_currentChipCtx->PC = addr - 2;
}

void CC8_SE_VX_BYTE(uint8_t x, uint8_t kk)
{
    s_currentChipCtx->PC += s_currentChipCtx->V[x] == kk ? 2 : 0;
}

void CC8_SNE_VX_BYTE(uint8_t x, uint8_t kk)
{
    s_currentChipCtx->PC += s_currentChipCtx->V[x] != kk ? 2 : 0;
}

void CC8_SE_VX_VY(uint8_t x, uint8_t y)
{
    s_currentChipCtx->PC += s_currentChipCtx->V[x] == s_currentChipCtx->V[y] ? 2 : 0;
}

void CC8_LD_VX_BYTE(uint8_t x, uint8_t kk)
{
    s_currentChipCtx->V[x] = kk;
}

void CC8_ADD_VX_BYTE(uint8_t x, uint8_t kk)
{
    s_currentChipCtx->V[x] = (s_currentChipCtx->V[x] + kk) & 0xFF;
}

void CC8_LD_VX_VY(uint8_t x, uint8_t y)
{
    s_currentChipCtx->V[x] = s_currentChipCtx->V[y];
}

void CC8_OR_VX_VY(uint8_t x, uint8_t y)
{
    s_currentChipCtx->V[x] |= s_currentChipCtx->V[y];
}

void CC8_AND_VX_VY(uint8_t x, uint8_t y)
{
    s_currentChipCtx->V[x] &= s_currentChipCtx->V[y];
}

void CC8_XOR_VX_VY(uint8_t x, uint8_t y)
{
    s_currentChipCtx->V[x] ^= s_currentChipCtx->V[y];
}

void CC8_ADD_VX_VY(uint8_t x, uint8_t y)
{
    uint16_t sum = s_currentChipCtx->V[x] + s_currentChipCtx->V[y];
    s_currentChipCtx->V[0X0F] = sum > 0xFF; // carry flag
    s_currentChipCtx->V[x] = sum & 0xFF;
}

void CC8_SUB_VX_VY(uint8_t x, uint8_t y)
{
    s_currentChipCtx->V[0x0F] = s_currentChipCtx->V[x] > s_currentChipCtx->V[y];
    s_currentChipCtx->V[x] -= s_currentChipCtx->V[y];
}

void CC8_SHR_VX_VY(uint8_t x, uint8_t y)
{
    s_currentChipCtx->V[0x0F] = s_currentChipCtx->V[x] & 0x01;
    s_currentChipCtx->V[x] >>= 1;
}

void CC8_SUBN_VX_VY(uint8_t x, uint8_t y)
{
    s_currentChipCtx->V[0x0F] = s_currentChipCtx->V[y] > s_currentChipCtx->V[x];
    s_currentChipCtx->V[x] = s_currentChipCtx->V[y] - s_currentChipCtx->V[x];
}

void CC8_SHL_VX_VY(uint8_t x, uint8_t y)
{
    s_currentChipCtx->V[0x0F] = (s_currentChipCtx->V[x] & 0x80) >> 7;
    s_currentChipCtx->V[x] <<= 1;
}

void CC8_SNE_VX_VY(uint8_t x, uint8_t y)
{
    s_currentChipCtx->PC += s_currentChipCtx->V[x] != s_currentChipCtx->V[y] ? 2 : 0;
}

void CC8_LD_I_ADDR(uint16_t addr)
{
    s_currentChipCtx->I = addr & 0xfff;
}

void CC8_JP_V0_ADDR(uint16_t addr)
{
    s_currentChipCtx->PC = addr + s_currentChipCtx->V[0];
}

void CC8_RND_VX_BYTE(uint8_t x, uint8_t kk)
{
    s_currentChipCtx->V[x] = (rand() % 0xFF) & kk;
}

void CC8_DRW_VX_VY_NIBBLE(uint8_t x, uint8_t y, uint8_t n)
{
    const uint16_t startAddress = s_currentChipCtx->I;
    s_currentChipCtx->V[0x0F] = 0;

    for (uint8_t byte = 0; byte < n; byte++)
    {
        for (uint8_t bit = 0; bit < 8; bit++)
        {
            uint8_t line = s_currentChipCtx->RAM[startAddress + byte];

            if ((line & (0x80 >> bit)) != 0)
            {
                uint8_t xPos = (s_currentChipCtx->V[x] + bit) % (CHIP_8_VRAM_WIDTH);
                uint8_t yPos = (s_currentChipCtx->V[y] + byte) % CHIP_8_VRAM_HEIGHT;
                uint16_t vramIndex = (yPos * CHIP_8_VRAM_WIDTH) + (xPos / 8);
                uint8_t vramBit = (xPos % 8);

                if ((s_currentChipCtx->VRAM[vramIndex] & (1 << vramBit)) != 0)
                {
                    s_currentChipCtx->V[0x0F] = 1;
                }

                s_currentChipCtx->VRAM[vramIndex] ^= (1 << vramBit);
            }
        }
    }
}

void CC8_SKP_VX(uint8_t x)
{
    s_currentChipCtx->PC += s_currentChipCtx->V[x] == s_currentChipCtx->KEYBOARD ? 2 : 0;
}

void CC8_SKNP_VX(uint8_t x)
{
    s_currentChipCtx->PC += s_currentChipCtx->V[x] != s_currentChipCtx->KEYBOARD ? 2 : 0;
}

void CC8_LD_VX_DT(uint8_t x)
{
    s_currentChipCtx->V[x] = s_currentChipCtx->DELAY;
}

void CC8_LD_VX_K(uint8_t x)
{
    while (s_currentChipCtx->KEYBOARD == 0);
    s_currentChipCtx->V[x] = s_currentChipCtx->KEYBOARD;
}

void CC8_LD_DT_VX(uint8_t x)
{
    s_currentChipCtx->DELAY = s_currentChipCtx->V[x];
}

void CC8_LD_ST_VX(uint8_t x)
{
    s_currentChipCtx->SOUND = s_currentChipCtx->V[x];
}

void CC8_ADD_I_VX(uint8_t x)
{
    s_currentChipCtx->I = (s_currentChipCtx->I + s_currentChipCtx->V[x]) & 0xFFFF;
}

void CC8_LD_F_VX(uint8_t x)
{
    s_currentChipCtx->I = s_currentChipCtx->V[x] * 5;
}

void CC8_LD_B_VX(uint8_t x)
{
    const uint8_t rawValue = s_currentChipCtx->V[x];

    const uint8_t hundreds = rawValue / 100;
    const uint8_t tens = (rawValue / 10) % 10;
    const uint8_t units = rawValue % 10;

    const uint16_t currentAddress = s_currentChipCtx->I;

    s_currentChipCtx->RAM[currentAddress] = hundreds;
    s_currentChipCtx->RAM[currentAddress + 1] = tens;
    s_currentChipCtx->RAM[currentAddress + 2] = units;
}

void CC8_LD_I_VX(uint8_t x)
{
    const uint16_t startAddress = s_currentChipCtx->I;
    const uint16_t endAddress = startAddress + x;

    for (uint16_t ramIndex = startAddress, vIndex = 0; ramIndex <= endAddress; ramIndex++, vIndex++)
    {
        s_currentChipCtx->RAM[ramIndex] = s_currentChipCtx->V[vIndex];
    }
}

void CC8_LD_VX_I(uint8_t x)
{
    const uint16_t startAddress = s_currentChipCtx->I;
    const uint16_t endAddress = startAddress + x;

    for (uint16_t ramIndex = startAddress, vIndex = 0; ramIndex <= endAddress; ramIndex++, vIndex++)
    {
        s_currentChipCtx->V[vIndex] = s_currentChipCtx->RAM[ramIndex];
    }
}
