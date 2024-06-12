#ifndef MINEMU_H
#define MINEMU_H
#include <stdint.h>
#include "3rd/khash.h"

#include "minemu/MNE_Flags.h"
#include "minemu/MNE_Log.h"
#include "minemu/MNE_File.h"
#include "minemu/MNE_Memory.h"

typedef enum {Keyboard, Window, System} ActionType;
typedef enum {Start, Stop, Reset, Quit} ShellAction;
typedef enum {Idle, Starting, Running, Stopped, Exception} ShellState;

//Callbacks
typedef void (*StepCallback)(unsigned int * pixels);
typedef void (*ActionCallback)(const char inputCode);
typedef void (*ShellCallback)(void * data);

//Models (TODO MOVE THESE...)
typedef struct 
{
    ActionType  type;
    uint8_t     code;
    uint8_t     value;
} ActionArg;

typedef struct {
    uint32_t frameWidth, frameHeight;
} ShellConfig;

typedef struct {
    char     name[32];
    uint32_t displayWidth, displayHeight;
    ShellConfig  UIConfig;
} EmulationInfo;

typedef struct 
{
    EmulationInfo  (*GetInfo)();
    uint8_t         (*Initialize)(int argc, const char ** argv);
    long            (*LoadProgram)(const char * filePath);
    void            (*QuitProgram)();
    int             (*TickEmulation)();
    void            (*TickTimers)();
    void            (*SetEmulationContext)(const void * context);
    void            (*OnRender)(uint32_t* pixels, const int64_t w, const int64_t h);
    void            (*OnInput)(const char code); //TODO: REFACTOR THIS TO USE A CUSTOM MODEL THAT HANDLES KEYBOARD,JOYSTICKS AND MOUSE
    void            (*Loop)(uint32_t frameTicks, uint32_t deltaTime);
} Emulation;

typedef struct
{
    void       (*Init)(void);
    void       (*UpdateFrame)(uint32_t *pixels);
    void       (*OnInput)(const char code);
    uint8_t    (*Shown)(void);
    void       (*ShellAction)(const ShellAction action, ShellCallback callback);
    void       (*SetState)(const ShellState state);
    ShellState (*GetState)();
} EmulatorShell;

typedef struct 
{       
    void    (*Init)(EmulationInfo *info, ActionCallback eventCallback, EmulatorShell *shell);
    uint8_t (*Render)(StepCallback renderCallback);
    uint32_t(*GetTicks)(void);
    void    (*Reset)(void);
    void    (*Exit)(void);
} EmuApp;

#endif