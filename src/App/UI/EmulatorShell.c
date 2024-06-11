// #include "../include/EmulatorShell.h"
#include "EmulatorShell.h"
#include <SDL2/SDL.h>
#include "Fonts/Default_7x8.h"


#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <math.h>

#define MAX_ROMS 128
#define MAX_SUB_FOLDERS 4

#define PATH_LENGHT 64
#define FRAME_WIDTH  300
#define FRAME_HEIGHT 128
#define MAX_SHELL_ACTIONS 4

static volatile uint8_t s_showMenu = 1;
static const uint32_t s_fontColor = 0X00FF00FF;
static const uint32_t s_backgroundColor = 0X1E1E1E1E;
static char s_files[MAX_ROMS][PATH_LENGHT]; // supports 128 paths with 64 characters wide path size
static char s_full_path[PATH_LENGHT];
static uint16_t s_readedRooms = 0;
static int s_tabsIndex = 0;
static int s_selectionIndex = 0;
static ShellCallback s_actions[MAX_SHELL_ACTIONS];
ShellState s_currentState;

void GetFolderContents(const char *folderPath)
{
    struct dirent *entry;
    DIR *directory = opendir(folderPath);
    uint8_t fileIndex = 0;

    if (directory)
    {
        while ((entry = readdir(directory)) != NULL && (fileIndex < MAX_ROMS))
        {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
            {
                strcpy(s_files[fileIndex], entry->d_name);

                size_t lenght = strlen(entry->d_name);
                s_files[fileIndex][lenght] = '\0';

                fileIndex++;
            }
        }

        s_readedRooms = fileIndex;
        closedir(directory);
    }
}


uint8_t ValidateDir(const char *dir)
{
    struct dirent *entry;
    DIR *directory = opendir(dir);
    uint8_t fileIndex = 0;

    if (!directory)
    {
        return 1;
    }
    else
    {
        strcat(s_full_path, dir);
        strcat(s_full_path, "/");

        GetFolderContents(dir);
        return 0;
    }
}

void EmuShell_DrawChar(unsigned char character, uint32_t *pixels, const int x, const int y)
{
    const uint16_t fontStartPos = (((uint8_t) character - 32) * 6);

    //TODO: OPTIMIZE THIS USING ONLY SEND DATA FUNCTION (REMOVE OVERHEAD LOOPS)
    for (uint8_t gylphIndex = 0; gylphIndex < 6; gylphIndex++)
    {
        for (uint8_t bit = 0; bit < 8; bit++)
        {
            pixels[((y + bit) % FRAME_HEIGHT) * FRAME_WIDTH + ((x + gylphIndex) % FRAME_WIDTH)] = (BitMapFont[fontStartPos + gylphIndex] >> bit) & 0x01 ? s_fontColor : s_backgroundColor;
        }
    }
}

void EmuShell_DrawString(const char *string, uint32_t * pixels, int x, int y)
{
  uint8_t index = 0;

  while (string[index])
  {
    EmuShell_DrawChar(string[index++], pixels, x + (index * 6), y);
  }
}

void DrawString()
{

}

void EmuShell_Init()
{
    // TODO: GET THIS VALUE FROM A CONFIG FILE AND MOVE THE STRCAT???
    strcat(s_full_path,CC8_ROMS_PATH);
    GetFolderContents(CC8_ROMS_PATH);
}

int ShellRow(int * yAxis)
{
   return *yAxis += DEFAUL_COL_Y_SPACING;
}

void EmuShell_UpdateFrame(uint32_t *pixels)
{
   // Starting row position...
   int rowYLoc = 1;
   int cursorYPos = 2;
   const uint8_t cursorBounds = (8 * 8);
   char currentRomStr[128];
   char currentStatusStr[32];

   // Clear rendering buffer
   memset(pixels, s_backgroundColor, FRAME_WIDTH * FRAME_HEIGHT * sizeof(uint32_t));

   EmuShell_DrawString("[MINEMU 0.1b BY XUL]", pixels, 32, ShellRow(&rowYLoc));

   switch (s_selectionIndex)
   {
        case 0: 
              if (s_currentState == Idle )
                strcpy(currentStatusStr, "[IDLE]");
              else
                strcpy(currentStatusStr, "[RUNNING]");
            break;
            
        case 1:
            if (s_currentState == Idle || s_currentState == Running)
            {
                char romPath[PATH_LENGHT];
                strcat(romPath,s_full_path);
                strcat(romPath, s_files[s_tabsIndex]);

                if (ValidateDir(romPath))
                {
                    s_actions[Start](romPath);
                }

                s_selectionIndex = 0;// antibounce
            }

            break;
   }

   sprintf(currentRomStr, "%s %s", currentStatusStr, s_files[s_tabsIndex]);
   EmuShell_DrawString(currentRomStr, pixels, 16, ShellRow(&rowYLoc) + 2);
   
   ShellRow(&rowYLoc);
   cursorYPos = rowYLoc + (2 * DEFAUL_COL_Y_SPACING);

   EmuShell_DrawString("ROMS:", pixels, 8, ShellRow(&rowYLoc));
   for (int i = 0; i < 8; i++)
   {
        int yLoc = ShellRow(&rowYLoc);
        int cursorY =  (cursorYPos + (s_tabsIndex * 8) % cursorBounds);
        EmuShell_DrawString("* ", pixels, 2, cursorY);
        EmuShell_DrawString(s_files[i], pixels, 16, yLoc);
   }
}

// BIG MF TODO: WTFFF I DID NOT WANT THIS...
uint16_t ClampValue(uint16_t value, uint16_t min, uint16_t max){
       if (value < min) {
        return min;
    } else if (value > max) {
        return max;
    } else {
        return value;
    }
}

// TODO: CHECK IF IS THIS PROCCESSED IN THE EVENT THREAD...
char lastCode;
void EmuShell_KeyPressed(const char code)
{
    if (lastCode == code) 
    {
        SDL_Delay(16);
        return;
    }

    switch (code)
    {
        // MENU
        case SDLK_ESCAPE:
            s_showMenu = !s_showMenu;
            break;

        // UP
        case SDLK_o:
            s_tabsIndex--;
            s_tabsIndex = ClampValue(s_tabsIndex, 0, s_readedRooms - 1);
            break;

        // DOWN
        case SDLK_l:
            s_tabsIndex++;
            s_tabsIndex =  ClampValue(s_tabsIndex, 0, s_readedRooms - 1);
            break;

        // CANCEL 
        case SDLK_k:
            s_selectionIndex++;
            s_selectionIndex = ClampValue(s_selectionIndex, 0, 0);
            break;

        // ACCEPT (MULTIPLE LEVELS)
        case SDLK_SEMICOLON:
            s_selectionIndex--;
            s_selectionIndex = ClampValue(s_selectionIndex, 0, 1);
            break;

        default:
            break;
    }

    lastCode = code;
}

uint8_t EmuShell_Shown()
{
    return s_showMenu;
}

void EmuShell_ShellAction(const ShellAction action, ShellCallback callback)
{
    if (action > MAX_SHELL_ACTIONS)
    {
        MNE_Log("Action: %i out of bounds(%i), skipping...\n", action, MAX_SHELL_ACTIONS);
        return;
    }

    s_actions[action] = callback;
}

void EmuShell_SetState(const ShellState state)
{
    s_currentState = state;
}

ShellState EmuShell_GetState()
{
    return s_currentState;
}
