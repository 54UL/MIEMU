#include "CC8_Emulator.h"

#include <stdlib.h>
#include <stdio.h>

static CC8_Machine *s_currentChipCtx = NULL;
static InstructionSet s_currentInstructions;

const uint8_t CC8_DEFAULT_FONT[] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // "0"
    0x20, 0x60, 0x20, 0x20, 0x70, // "1"
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // "2"
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // "3"
    0x90, 0x90, 0xF0, 0x10, 0x10, // "4"
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // "5"
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // "6"
    0xF0, 0x10, 0x20, 0x40, 0x40, // "7"
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // "8"
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // "9"
    0xF0, 0x90, 0xF0, 0x90, 0x90, // "A"
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // "B"
    0xF0, 0x80, 0x80, 0x80, 0xF0, // "C"
    0xE0, 0x90, 0x90, 0x90, 0xE0, // "D"
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // "E"
    0xF0, 0x80, 0xF0, 0x80, 0x80  // "F"
};

void CC8_BuildInstructionLUT()
{
    // Compute instructions LUT

    // BASE CHIP 8 INSTRUCTIONS...
    s_currentInstructions.Set(0x0000, NULL); // NOP
    s_currentInstructions.Set(0x00E0, CC8_CLS);
    s_currentInstructions.Set(0x00EE, CC8_RET);
    s_currentInstructions.Set(0x1000, CC8_JMP);
    s_currentInstructions.Set(0x2000, CC8_CALL);
    s_currentInstructions.Set(0x3000, CC8_SE_VX_BYTE);
    s_currentInstructions.Set(0x4000, CC8_SNE_VX_BYTE); 
    s_currentInstructions.Set(0x5000, CC8_SE_VX_VY); 
    s_currentInstructions.Set(0x6000, CC8_LD_VX_BYTE); 
    s_currentInstructions.Set(0x7000, CC8_ADD_VX_BYTE); 
    s_currentInstructions.Set(0x8000, CC8_LD_VX_VY); 
    s_currentInstructions.Set(0x8001, CC8_OR_VX_VY); 
    s_currentInstructions.Set(0x8002, CC8_AND_VX_VY); 
    s_currentInstructions.Set(0x8003, CC8_XOR_VX_VY); 
    s_currentInstructions.Set(0x8004, CC8_ADD_VX_VY); 
    s_currentInstructions.Set(0x8005, CC8_SUB_VX_VY); 
    s_currentInstructions.Set(0x8006, CC8_SHR_VX_VY); 
    s_currentInstructions.Set(0x8007, CC8_SUBN_VX_VY); 
    s_currentInstructions.Set(0x800E, CC8_SHL_VX_VY); 
    s_currentInstructions.Set(0x9000, CC8_SNE_VX_VY); 
    s_currentInstructions.Set(0xA000, CC8_LD_I_ADDR); 
    s_currentInstructions.Set(0xB000, CC8_JP_V0_ADDR); 
    s_currentInstructions.Set(0xC000, CC8_RND_VX_BYTE); 
    s_currentInstructions.Set(0xD000, CC8_DRW_VX_VY_NIBBLE);
    s_currentInstructions.Set(0xE09E, CC8_SKP_VX);
    s_currentInstructions.Set(0xE0A1, CC8_SKNP_VX);
    s_currentInstructions.Set(0xF007, CC8_LD_VX_DT);
    s_currentInstructions.Set(0xF00A, CC8_LD_VX_K);
    s_currentInstructions.Set(0xF015, CC8_LD_DT_VX);
    s_currentInstructions.Set(0xF018, CC8_LD_ST_VX);
    s_currentInstructions.Set(0xF01E, CC8_ADD_I_VX);
    s_currentInstructions.Set(0xF029, CC8_LD_F_VX);
    s_currentInstructions.Set(0xF033, CC8_LD_B_VX);
    s_currentInstructions.Set(0xF055, CC8_LD_I_VX);
    s_currentInstructions.Set(0xF065, CC8_LD_VX_I);

    // TODO: ADD S-CHIP8 instructions below...
}

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

void CC8_DebugMachine(CC8_Machine *machine, uint8_t enable)
{
    if (!enable)
        return;

    printf("| %-10s | %-10s |\n", "Name", "Value");
    printf("|------------|------------|\n");
    printf("| %-10s | %-#10x |\n", "RAM", (unsigned int)machine->RAM);
    printf("| %-10s | %-#10x |\n", "V", (unsigned int)machine->V);
    printf("| %-10s | %-#10x |\n", "SOUND", (unsigned int)machine->SOUND);
    printf("| %-10s | %-#10x |\n", "DELAY", (unsigned int)machine->DELAY);
    printf("| %-10s | %-#10x |\n", "I", (unsigned int)machine->I);
    printf("| %-10s | %-#10x |\n", "PC", (unsigned int)machine->PC);
    printf("| %-10s | %-#10x |\n", "SP", (unsigned int)machine->SP);
    printf("| %-10s | %-#10x |\n", "STACK", (unsigned int)machine->STACK);
    printf("| %-10s | %-#10x |\n", "VRAM", (unsigned int)machine->VRAM);
}

void CC8_LoadProgram(const char *filePath)
{
    FILE *file = fopen(filePath, "rb");
    if (file == NULL)
    {
        printf("Unable to open file %s\n", filePath);
        return;
    }

    // Get the size of the file
    fseek(file, 0L, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0L, SEEK_SET);

    // Allocate a buffer for reading the file
    uint8_t *buffer = (uint8_t *)calloc(1, file_size + 1);
    size_t bytes_read = fread(buffer, 1, file_size, file);
    s_currentChipCtx->PROGRAM_SIZE = bytes_read;

    // LOAD PROGRAM AND FONT INTO RAM
    const size_t font_addr_start = 0x000; // The first thing in memory is the font
    const size_t boot_addr_start = 0x200; // 512

    uint16_t addr = 0;
    uint16_t loop_index = 0;

    for (addr = boot_addr_start; (addr < boot_addr_start + bytes_read); addr++)
    {
        s_currentChipCtx->RAM[addr] = buffer[loop_index++];
    }

    s_currentChipCtx->PC = boot_addr_start;

    loop_index = 0;
    printf("Loaded font size: %i\n", sizeof(CC8_DEFAULT_FONT));
    for (addr = font_addr_start; (addr < font_addr_start + sizeof(CC8_DEFAULT_FONT)); addr++)
    {
        s_currentChipCtx->RAM[font_addr_start + addr] = CC8_DEFAULT_FONT[loop_index++];
    }

    // Print the buffer (TODO:addd flag to print this thing)
    // HexDump(buffer, bytes_read);

    // Clean up resources
    free(buffer);
    fclose(file);
}

void CC8_QuitProgram()
{
    if (s_currentChipCtx != NULL)
        free(s_currentChipCtx);
}

void CC8_Step(uint16_t opcode)
{
    InstructionContext ctx;
    instructionFnPtr * fetchedInstruction;
    
    // Instruction decoding
    ctx->x = (opcode >> 8) & 0x0F;
    ctx->y = (opcode >> 4) & 0x0F;
    ctx->nnn = opcode & 0x0FFF;
    ctx->kk = opcode & 0x00FF;
    ctx->n = opcode & 0x000F;

    // Instruction fetching
    fetchedInstruction = s_currentInstructions.Fetch(opcode);
    
    // Instruction execution
    if (fetchedInstruction != NULL)
    {
        *fetchedInstruction(ctx);
    }
    else
    {
        printf("Invalid instruction: %06X\n", opcode);
    }
}

void CC8_TickDelayTimer()
{
    if (s_currentChipCtx->DELAY != 0)
        s_currentChipCtx->DELAY--;
}

int CC8_TickEmulation()
{
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