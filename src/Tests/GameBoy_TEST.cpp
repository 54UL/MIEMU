/*
TODO WALL:
    - SEPARTE LOAD/COPY/STORE INSTRUCTIONS INTO INDEPEDENT TEST CASES FROM LoadTests8B
    - IMPLEMENT CORNER CASES AND CB PREFIX TESTS... (OH GOOD PLEASE HAVE MERCY ON ME)
    - Refactors needed and a header file...
    - Rename this test file to 'GameBoyCpuTest'
    - Make expectMessage only to return the name of the function...

RESOURCES:
- TEST ROOMS EMULATION REFERENCES:
    - https://github.com/Hacktix/BullyGB/wiki
    - http://blargg.8bitalley.com/parodius/gb-tests/
*/

// #define TEST_ROOM_PATH "../../../ROMS/GameBoy/blargg-ld_r_r.gb"
#define ExpectMessage(instr, message) "GB INSTRUCTION [" << #instr << "] EXPECTED: " << #message;
#define GB_DEBUG

#include <gtest/gtest.h>
#include <stdlib.h>
extern "C"
{
#include <minemu.h>
#include <GameBoy.h>
}

class GameBoyFixture : public testing::Test
{
protected:
    Emulation *emulator;
    EmulationState *emulationCtx;

    void SetUp() override
    {
        // Initialize the emulation API
        MNE_New(emulationCtx, 1, EmulationState);

        emulator = &GameBoyEmulator;
        emulator->SetEmulationContext(static_cast<void *>(emulationCtx));
        emulator->Initialize(NULL, NULL);
    }

    void TearDown() override
    {
        // Dispose emulation resources here
        emulator->QuitProgram();
        MNE_Delete(emulationCtx);
    }
};

void RunProgram(const Emulation *emulator, const EmulationState *emulationCtx, const uint8_t *programArray, const uint16_t programLength);

void LoadTests8B(const Emulation *emulator, const EmulationState *emulationCtx);
void CpuControlTests(const Emulation *emulator, const EmulationState *emulationCtx);
void CpuAluTests(const Emulation *emulator, const EmulationState *emulationCtx);
void Cpu16BitAluTests(const Emulation *emulator, const EmulationState *emulationCtx);

void RunProgram(const Emulation *emulator, const EmulationState *emulationCtx, const uint8_t *programArray, const uint16_t programLength)
{
    bool executionStatus = true;
    MNE_Log("\n-----------------PROGRAM START---------------\n");

    // Memset to 0 using a harcoded range of 0xFF (:skull:)
    constexpr uint16_t cleanUpRange = 0XFF;
    for (int i = 0; i < cleanUpRange; i++)
    {
        emulationCtx->memory[i] = 0x00;
    }
    // // Clean registers
    // for (int i = 0; i < 4; i++)
    // {
    //       emulationCtx->registers->CPU[i] = 0;
    // }

    // Restart pc
    emulationCtx->registers->PC = 0;

    // From the start of the program, later on it will have to be pc...
    // const uint16_t pos = emulationCtx->registers->PC; // might this be useful for testing previous instructions states so thats why is here
    const uint16_t pos = 0;


    // Copy program into current pc position (just for testing...)
    for (int i = pos; i < (pos + programLength); i++)
    {
        emulationCtx->memory[i] = programArray[i];
    }

    // Process instructions...
    for (int i = 0; i < programLength; i++)
    {
        // Step emulation
        emulator->TickEmulation();
        // Check if processed op code was executed fine
        EXPECT_TRUE(emulationCtx->registers->INSTRUCTION != GB_INVALID_INSTRUCTION);
    }

    MNE_Log("-----------------PROGRAM END-----------------\n");
}

void LoadTests8B(const Emulation *emulator, const EmulationState *emulationCtx)
{
    constexpr uint8_t testValue = 0x12;
    // LD_R_N
    constexpr uint8_t loadA[] = {0x3E, testValue}; 
    // constexpr uint8_t loadB[] = {0x06, testValue};
    // constexpr uint8_t loadC[] = {0x0E, testValue};
    // constexpr uint8_t loadD[] = {0x16, testValue};
    // constexpr uint8_t loadE[] = {0x1E, testValue};
    // constexpr uint8_t loadH[] = {0x26, testValue};
    // constexpr uint8_t loadL[] = {0x2E, testValue};

    // LD_R_R
    constexpr uint8_t copyAToB[] = {0x78};
    // constexpr uint8_t copyCToD[] = {0x4A};
    // constexpr uint8_t copyEToH[] = {0x5C};
    // constexpr uint8_t copyLToA[] = {0x6F};
    // LD_R_HL 
    constexpr uint8_t loadAFromHL[] = {0x7E}; // LD A,(HL) - 0x7E
    // constexpr uint8_t loadBFromDE[] = {0x1A};
    // constexpr uint8_t storeBAtHL[] = {0x70};
    // constexpr uint8_t storeCAtDE[] = {0x41};

    constexpr uint8_t loadAFromHLPlus1[] = {0x86, 0x01};
    constexpr uint8_t loadBFromDEMinus2[] = {0x56, 0xFE};
    
    constexpr uint8_t storeCAtHLPlus3[] = {0x71, 0x03};
    constexpr uint8_t storeDAtDEMinus4[] = {0x51, 0xFC};

    constexpr uint8_t storeEFAtHL[] = {0x36, 0xEF};
    constexpr uint8_t storeCDAtDE[] = {0x12, 0xCD};

    // Run the program loadA
    RunProgram(emulator, emulationCtx, loadA, sizeof(loadA));
    EXPECT_TRUE(emulationCtx->registers->CPU[GB_AF_OFFSET] >> 8 == testValue) << ExpectMessage(loadA, testValue);

    // Run the program copyAToB
    RunProgram(emulator, emulationCtx, copyAToB, sizeof(copyAToB));
    EXPECT_TRUE((emulationCtx->registers->CPU[GB_AF_OFFSET] >> 8) == (emulationCtx->registers->CPU[GB_BC_OFFSET] >> 8)) << ExpectMessage(copyAToB, "B = A");

    // Run the program loadAFromHL
    emulationCtx->memory[0XF000]  = testValue;
    emulationCtx->registers->CPU[GB_HL_OFFSET] = 0XF000;

    RunProgram(emulator, emulationCtx, loadAFromHL, sizeof(loadAFromHL));
    EXPECT_TRUE(emulationCtx->registers->CPU[GB_AF_OFFSET] >> 8 == testValue) << ExpectMessage(loadAFromHL, "A = (HL)");
}

// TODO: COMPLETE CASES...
void CpuControlTests(const Emulation *emulator, const EmulationState *emulationCtx)
{
    // Jump instructions
    constexpr uint8_t jumpInstruction[] = {0xC3, 0x10, 0x00};   // JP 0x0010
    constexpr uint8_t jumpRelativeInstruction[] = {0x18, 0x0F}; // JR 0x000F
    // Call and Return instructions
    constexpr uint8_t callInstruction[] = {0xCD, 0x20, 0x00}; // CALL 0x0020
    constexpr uint8_t returnInstruction[] = {0xC9};           // RET
    // Conditional jump instructions
    constexpr uint8_t conditionalJumpInstruction[] = {0xC2, 0x30, 0x00};   // JP NZ, 0x0030
    constexpr uint8_t conditionalJumpRelativeInstruction[] = {0x20, 0x0F}; // JR NZ, 0x000F
    // Conditional call and return instructions
    constexpr uint8_t conditionalCallInstruction[] = {0xC4, 0x40, 0x00}; // CALL NZ, 0x0040
    constexpr uint8_t conditionalReturnInstruction[] = {0xC0};           // RET NZ
    // Restart (RST) instructions
    constexpr uint8_t restartInstruction[] = {0xC7}; // RST 00H

    // Jump instructions
    RunProgram(emulator, emulationCtx, jumpInstruction, sizeof(jumpInstruction));
    EXPECT_TRUE(emulationCtx->registers->PC == 0x0010) << "Jump instruction";
    RunProgram(emulator, emulationCtx, jumpRelativeInstruction, sizeof(jumpRelativeInstruction));
    EXPECT_TRUE(emulationCtx->registers->PC == 0x0021) << "Jump Relative instruction";

    // Call and Return instructions
    RunProgram(emulator, emulationCtx, callInstruction, sizeof(callInstruction));
    EXPECT_TRUE(emulationCtx->registers->PC == 0x0020) << "Call instruction";
    RunProgram(emulator, emulationCtx, returnInstruction, sizeof(returnInstruction));
    EXPECT_TRUE(emulationCtx->registers->PC == 0x0013) << "Return instruction";

    // Conditional jump instructions
    // emulationCtx->registers->CPU[GB_ZF_OFFSET] = 0;  // Set Zero Flag to false
    // RunProgram(emulator, emulationCtx, conditionalJumpInstruction, sizeof(conditionalJumpInstruction));
    // EXPECT_TRUE(emulationCtx->registers->PC == 0x0030) << "Conditional Jump instruction";
    // emulationCtx->registers->CPU[GB_ZF_OFFSET] = 1;  // Set Zero Flag to true
    // RunProgram(emulator, emulationCtx, conditionalJumpRelativeInstruction, sizeof(conditionalJumpRelativeInstruction));
    // EXPECT_TRUE(emulationCtx->registers->PC == 0x0033) << "Conditional Jump Relative instruction";

    // Conditional call and return instructions
    RunProgram(emulator, emulationCtx, conditionalCallInstruction, sizeof(conditionalCallInstruction));
    EXPECT_TRUE(emulationCtx->registers->PC == 0x0040) << "Conditional Call instruction";
    RunProgram(emulator, emulationCtx, conditionalReturnInstruction, sizeof(conditionalReturnInstruction));
    EXPECT_TRUE(emulationCtx->registers->PC == 0x0043) << "Conditional Return instruction";

    // Restart (RST) instructions
    RunProgram(emulator, emulationCtx, restartInstruction, sizeof(restartInstruction));
    EXPECT_TRUE(emulationCtx->registers->PC == 0x0000) << "Restart instruction";
}

// TODO: COMPLETE CASES...
void CpuAluTests(const Emulation *emulator, const EmulationState *emulationCtx)
{
    // ALU instructions
    constexpr uint8_t testValue = 0x12;
    // 0x3E MEANS LD A,D8...
    constexpr uint8_t addInstruction[] = {0x3E, testValue, 0x87}; // ADD A, A
    constexpr uint8_t adcInstruction[] = {0x3E, testValue, 0x8F}; // ADC A, A
    constexpr uint8_t subInstruction[] = {0x3E, testValue, 0x97}; // SUB A, A
    constexpr uint8_t sbcInstruction[] = {0x3E, testValue, 0x9F}; // SBC A, A
    constexpr uint8_t andInstruction[] = {0x3E, testValue, 0xA7}; // AND A, A
    constexpr uint8_t xorInstruction[] = {0x3E, testValue, 0xAF}; // XOR A, A
    constexpr uint8_t orInstruction[] = {0x3E, testValue, 0xB7};  // OR A, A
    constexpr uint8_t cpInstruction[] = {0x3E, testValue, 0xBF};  // CP A, A

    // ALU instructions
    RunProgram(emulator, emulationCtx, addInstruction, sizeof(addInstruction));
    EXPECT_TRUE(emulationCtx->registers->CPU[GB_AF_OFFSET] >> 8 == testValue + testValue) << "ADD A, A";

    RunProgram(emulator, emulationCtx, adcInstruction, sizeof(adcInstruction));
    EXPECT_TRUE(emulationCtx->registers->CPU[GB_AF_OFFSET] >> 8 == (testValue + testValue + 1)) << "ADC A, A";

    RunProgram(emulator, emulationCtx, subInstruction, sizeof(subInstruction));
    EXPECT_TRUE(emulationCtx->registers->CPU[GB_AF_OFFSET] >> 8 == 0x00) << "SUB A, A";

    RunProgram(emulator, emulationCtx, sbcInstruction, sizeof(sbcInstruction));
    EXPECT_TRUE(emulationCtx->registers->CPU[GB_AF_OFFSET] >> 8 == 0xFF) << "SBC A, A";

    RunProgram(emulator, emulationCtx, andInstruction, sizeof(andInstruction));
    EXPECT_TRUE(emulationCtx->registers->CPU[GB_AF_OFFSET] >> 8 == testValue) << "AND A, A";

    RunProgram(emulator, emulationCtx, xorInstruction, sizeof(xorInstruction));
    EXPECT_TRUE(emulationCtx->registers->CPU[GB_AF_OFFSET] >> 8 == 0x00) << "XOR A, A";

    RunProgram(emulator, emulationCtx, orInstruction, sizeof(orInstruction));
    EXPECT_TRUE(emulationCtx->registers->CPU[GB_AF_OFFSET] >> 8 == testValue) << "OR A, A";

    RunProgram(emulator, emulationCtx, cpInstruction, sizeof(cpInstruction));
    EXPECT_TRUE(emulationCtx->registers->CPU[GB_AF_OFFSET] >> 8 == 0x00) << "CP A, A";
}

void Cpu16BitAluTests(const Emulation *emulator, const EmulationState *emulationCtx)
{
    constexpr uint8_t testValueLow = 0x12;
    constexpr uint8_t testValueHigh = 0x34;

    // 16-bit ALU instructions
    constexpr uint8_t ldHLInstruction[] = {0x21, testValueLow, testValueHigh}; // LD HL, nn
    constexpr uint8_t addHLBCInstruction[] = {0x01, 0xCD, 0xAB, 0x09};         // LD BC, 0xABCD ; ADD HL, BC
    constexpr uint8_t addHLDEInstruction[] = {0x11, 0xEF, 0xCD, 0x19};         // LD DE, 0xCDEF ; ADD HL, DE
    constexpr uint8_t addHLHLInstruction[] = {0x21, 0x34, 0x12, 0x29};         // LD HL, 0x1234 ; ADD HL, HL
    constexpr uint8_t addHLSPInstruction[] = {0x31, 0x67, 0x45, 0x39};         // LD SP, 0x4567 ; ADD HL, SP
    constexpr uint8_t incHLInstruction[] = {0x21, 0xFF, 0xFF, 0x23};           // LD HL, 0xFFFF ; INC HL
    constexpr uint8_t decHLInstruction[] = {0x21, 0x00, 0x01, 0x2B};           // LD HL, 0x0001 ; DEC HL

    // 16-bit ALU instructions
    RunProgram(emulator, emulationCtx, ldHLInstruction, sizeof(ldHLInstruction));
    EXPECT_TRUE(emulationCtx->registers->CPU[GB_HL_OFFSET] == (testValueHigh << 8 | testValueLow)) << "LD HL, nn";

    RunProgram(emulator, emulationCtx, addHLBCInstruction, sizeof(addHLBCInstruction));
    EXPECT_TRUE(emulationCtx->registers->CPU[GB_HL_OFFSET] == 0xDFBC) << "ADD HL, BC";

    RunProgram(emulator, emulationCtx, addHLDEInstruction, sizeof(addHLDEInstruction));
    EXPECT_TRUE(emulationCtx->registers->CPU[GB_HL_OFFSET] == 0x210B) << "ADD HL, DE";

    RunProgram(emulator, emulationCtx, addHLHLInstruction, sizeof(addHLHLInstruction));
    EXPECT_TRUE(emulationCtx->registers->CPU[GB_HL_OFFSET] == 0x2468) << "ADD HL, HL";

    RunProgram(emulator, emulationCtx, addHLSPInstruction, sizeof(addHLSPInstruction));
    EXPECT_TRUE(emulationCtx->registers->CPU[GB_HL_OFFSET] == 0x6ACE) << "ADD HL, SP";

    RunProgram(emulator, emulationCtx, incHLInstruction, sizeof(incHLInstruction));
    EXPECT_TRUE(emulationCtx->registers->CPU[GB_HL_OFFSET] == 0x0000) << "INC HL";

    RunProgram(emulator, emulationCtx, decHLInstruction, sizeof(decHLInstruction));
    EXPECT_TRUE(emulationCtx->registers->CPU[GB_HL_OFFSET] == 0xFFFF) << "DEC HL";
}

// Test case for 8-bit load/store instructions
TEST_F(GameBoyFixture, LoadStore8BitInstructions)
{
    LoadTests8B(emulator, emulationCtx);
}

// Test case for Game Boy CPU control instructions
// TEST_F(GameBoyFixture, ControlInstructions)
// {
//     CpuControlTests(emulator, emulationCtx);
// }

// Test case for 8-bit ALU instructions
// TEST_F(GameBoyFixture, Alu8BitInstructions)
// {
//     CpuAluTests(emulator, emulationCtx);
// }

// Test case for 8-bit ALU instructions
// TEST_F(GameBoyFixture, Alu16BitInstructions)
// {
//     Cpu16BitAluTests(emulator, emulationCtx);
// }