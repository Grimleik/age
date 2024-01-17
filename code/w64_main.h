#if !defined(W64_MAIN_H)
/* ========================================================================
   Creator: Grimleik $
   ========================================================================*/
#define W64_MAIN_H
    
#include <Windows.h>
#include "age.h"

struct gameCode_t {
    HMODULE         gameCodeDLL;
    char           *gameCodeDLLName;
    GameInit_t     *gameInit;
    GameUpdate_t   *gameUpdate;
    GameShutdown_t *gameShutdown;
    b32             isValid;
    FILETIME        lastDLLWriteTime;
};

struct w64State_t {
    gameCode_t gameCode;
    char      *sourceDLL;
    char      *targetDLL;
    char      *pdbLockFile;
    s32        windowWidth;
    s32        windowHeight;
    HWND       hwnd;
};

#endif
