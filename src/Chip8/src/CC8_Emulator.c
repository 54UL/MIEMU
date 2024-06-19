#include <stdlib.h>
#include <stdio.h>
#include <CC8_Emulator.h>
#include <CC8_Instructions.h>
#include <minemu.h>


static CC8_Memory * s_currentChipCtx;
static int columCount =0; // TODO: remove this debug var

typedef struct {
    uint16_t mask;
    uint16_t opcode;
    instructionFnPtr handler;
} Instruction;

static Instruction s_instructionSet[CC8_INSTRUCTION_SET_LENGHT] = 
{
    // MASK, OPCODE, HANDLER
    {0XFFFF, 0X00E0, (instructionFnPtr) CC8_CLS},
    {0XFFFF, 0x00EE, (instructionFnPtr) CC8_RET},
    {0xF000, 0x1000, (instructionFnPtr) CC8_JMP},
    {0xF000, 0x2000, (instructionFnPtr) CC8_CALL},
    {0xF000, 0x3000, (instructionFnPtr) CC8_SE_VX_BYTE},
    {0xF000, 0x4000, (instructionFnPtr) CC8_SNE_VX_BYTE},
    {0xF000, 0x5000, (instructionFnPtr) CC8_SE_VX_VY},
    {0xF000, 0x6000, (instructionFnPtr) CC8_LD_VX_BYTE},
    {0xF000, 0x7000, (instructionFnPtr) CC8_ADD_VX_BYTE},
    {0xF00F, 0x8000, (instructionFnPtr) CC8_LD_VX_VY},
    {0xF00F, 0x8001, (instructionFnPtr) CC8_OR_VX_VY},
    {0xF00F, 0x8002, (instructionFnPtr) CC8_AND_VX_VY},
    {0xF00F, 0x8003, (instructionFnPtr) CC8_XOR_VX_VY},
    {0xF00F, 0x8004, (instructionFnPtr) CC8_ADD_VX_VY},
    {0xF00F, 0x8005, (instructionFnPtr) CC8_SUB_VX_VY},
    {0xF00F, 0x8006, (instructionFnPtr) CC8_SHR_VX_VY},
    {0xF00F, 0x8007, (instructionFnPtr) CC8_SUBN_VX_VY},
    {0xF00F, 0x800E, (instructionFnPtr) CC8_SHL_VX_VY},
    {0xF000, 0x9000, (instructionFnPtr) CC8_SNE_VX_VY},
    {0xF000, 0xA000, (instructionFnPtr) CC8_LD_I_ADDR},
    {0xF000, 0xB000, (instructionFnPtr) CC8_JP_V0_ADDR},
    {0xF000, 0xC000, (instructionFnPtr) CC8_RND_VX_BYTE},
    {0xF000, 0xD000, (instructionFnPtr) CC8_DRW_VX_VY_NIBBLE},
    {0XF0FF, 0xE09E, (instructionFnPtr) CC8_SKP_VX},
    {0XF0FF, 0xE0A1, (instructionFnPtr) CC8_SKNP_VX},
    {0xF00F, 0xF007, (instructionFnPtr) CC8_LD_VX_DT},
    {0xF00F, 0xF00A, (instructionFnPtr) CC8_LD_VX_K},
    {0XF0FF, 0xF015, (instructionFnPtr) CC8_LD_DT_VX},
    {0XF0FF, 0xF018, (instructionFnPtr) CC8_LD_ST_VX},
    {0XF0FF, 0xF01E, (instructionFnPtr) CC8_ADD_I_VX},
    {0XF0FF, 0xF029, (instructionFnPtr) CC8_LD_F_VX},
    {0XF0FF, 0xF033, (instructionFnPtr) CC8_LD_B_VX},
    {0XF0FF, 0xF055, (instructionFnPtr) CC8_LD_I_VX},
    {0XF0FF, 0xF065, (instructionFnPtr) CC8_LD_VX_I}
    // Super chip 8 instructions
};

instructionFnPtr FetchInstruction(uint16_t opcode)
{
    // Implement hashing!!!
    for (int  i = 0; i < CC8_INSTRUCTION_SET_LENGHT ; i++)
    {
        uint16_t opmask = (opcode & s_instructionSet[i].mask);

        if ((opmask == s_instructionSet[i].opcode))
        {
            return s_instructionSet[i].handler;
        }
    }

    return NULL;
}

EmulationInfo CC8_GetInfo()
{
    EmulationInfo info;
    strcpy(info.name,"MINEMU-CHIP8");

    info.displayWidth = CHIP_8_VRAM_WIDTH;
    info.displayHeight = CHIP_8_VRAM_HEIGHT;
    info.displayScaleFactor = 8; // Calculate this value based on dpi 
    info.UIConfig.frameWidth = 300;
    info.UIConfig.frameHeight = 128;
    
    return info;
}

void CC8_PopulateMemory(const uint8_t *buffer, size_t bytesRead)
{
    // LOAD PROGRAM
    uint16_t addr = 0;
    uint16_t loop_index = 0;
    for (addr = CC8_BOOT_ADDR_START; (addr < CC8_BOOT_ADDR_START + bytesRead); addr++)
    {
        s_currentChipCtx->RAM[addr] = buffer[loop_index++];
    }

    // LOAD FONT
    loop_index = 0;
    MNE_Log("Loaded font size: %li\n", sizeof(CC8_FONT));
    for (addr = CC8_FONT_ADDR_START; (addr < CC8_FONT_ADDR_START + sizeof(CC8_FONT)); addr++)
    {
        s_currentChipCtx->RAM[CC8_FONT_ADDR_START + addr] = CC8_FONT[loop_index++];
    }

    s_currentChipCtx->PC = CC8_BOOT_ADDR_START;
}

long CC8_LoadProgram(const char *filePath)
{
    if (s_currentChipCtx == NULL)
    {
        MNE_New(s_currentChipCtx, 1, CC8_Memory);
        CC8_SetEmulationContext((void *) s_currentChipCtx);
    }

    return MNE_ReadFile(filePath, MNE_HEX_DUMP_FILE_FLAG, CC8_PopulateMemory);
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
    InstructionContext ctx;

    if (opcode == 0x0000) // NOP
    { 
        s_currentChipCtx->INSTRUCTION = 0x0000;
        return; 
    }

    // Instruction decoding
    ctx.x = (opcode >> 8) & 0x0F;
    ctx.y = (opcode >> 4) & 0x0F;
    ctx.nnn = opcode & 0x0FFF;
    ctx.kk = opcode & 0x00FF;
    ctx.n = opcode & 0x000F;
    ctx.memory = s_currentChipCtx;

    // Instruction fetching
    instructionFnPtr fetchedInstruction = FetchInstruction(opcode);
    
    // Instruction execution
    if (fetchedInstruction != NULL)
    {
        MNE_Log("[%04X] ", opcode);
        if (columCount++ >= 16)
        {
            MNE_Log("\n");
            columCount = 0;
        }

        fetchedInstruction(&ctx);
        s_currentChipCtx->INSTRUCTION = opcode; // Stores executed opcode to check later if was running fine
    }
    else
    {
        MNE_Log("[Invalid opcode: %04X]\n", opcode);
        columCount = 0;
        s_currentChipCtx->INSTRUCTION = CC8_INVALID_INSTRUCTION; // Invalidate last instruction entry
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

void CC8_SetEmulationContext(const void *context)
{
    s_currentChipCtx = (CC8_Memory *) context;
}

void CC8_OnInput(const char code)
{
    if (s_currentChipCtx == NULL) return;
    s_currentChipCtx->KEYBOARD = code;
}

void CC8_OnRender(uint32_t* pixels, const int64_t w, const int64_t h)
{
    if (s_currentChipCtx == NULL) return;

    for (int i = 0; i < CHIP_8_VRAM_HEIGHT; i++)
    {
        for (int j = 0; j < CHIP_8_VRAM_WIDTH; j++)
        {
            int byteIndex = (i * CHIP_8_VRAM_WIDTH + (j / 8));
            int bitIndex = j % 8;
            uint8_t vramByte = s_currentChipCtx->VRAM[byteIndex];
            uint8_t vramBit = (vramByte >> bitIndex) & 0x1;

            pixels[i * CHIP_8_VRAM_WIDTH + j] = vramBit ? CHIP_8_FOREGROUND_DISPLAY_COLOR : CHIP_8_BACKGROUND_DISPLAY_COLOR;
        }
    }
}

// TODO: REMOVE ALL STATIC VARS FROM ALL THE CODE!!!
static uint32_t last_update_emulation;
static uint32_t last_update_time_timers;

void CC8_Loop(uint32_t currentTime, uint32_t deltaTime)
{
    last_update_emulation += deltaTime;
    last_update_time_timers += deltaTime;
    
    if (last_update_time_timers > 16) // TIMERS FREQ IN MS
    { 
        CC8_TickDelayTimer();
        last_update_time_timers = 0;
    }

    if (last_update_emulation >= 2 ) // CPU FREQ IN MS
    {
        CC8_TickEmulation();
        last_update_emulation = 0;
    }

   
}