/* ========================================================================
   Creator: Grimleik $
   ========================================================================*/
#include "age.h"

#include <Windows.h>
#include <dwmapi.h>
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "Dwmapi.lib")

typedef struct gameCode_t {
    HMODULE         gameCodeDLL;
    char           *gameCodeDLLName;
    GameInit_t     *gameInit;
    GameUpdate_t   *gameUpdate;
    GameShutdown_t *gameShutdown;
    b32             isValid;
    FILETIME        lastDLLWriteTime;
} gameCode_t;

typedef struct w64_drawbuffer_t {
    void *memory;
    s32   width;
    s32   height;
    u32   pitch;
    u32   bytesPerPixel;
} w64_drawbuffer_t;

typedef struct w64_rendering_t {
    HDC              deviceContext;
    w64_drawbuffer_t backBuffer;
    BITMAPINFO       backBufferInfo;
    b32              vsync;
} w64_rendering_t;

typedef struct w64_state_t {
    gameCode_t gameCode;
    char      *sourceDLL;
    char      *targetDLL;
    char      *pdbLockFile;
    s32        windowWidth;
    s32        windowHeight;
    HWND       hwnd;
} w64_state_t;

void CopyStrings(char *source, char *dest, size_t size) {
    for (int i = 0; i < size; ++i)
        *dest++ = *source++;
    *dest = '\0';
}

size_t CatStrings(size_t SourceACount, char *SourceA,
                  size_t SourceBCount, char *SourceB,
                  size_t DestCount, char *Dest) {
    // TODO(casey): Dest bounds checking!
    for (int Index = 0;
         Index < SourceACount;
         ++Index) {
        *Dest++ = *SourceA++;
    }

    for (int Index = 0;
         Index < SourceBCount;
         ++Index) {
        *Dest++ = *SourceB++;
    }

    *Dest++ = 0;
    return SourceACount + SourceBCount;
}

FILETIME GetFileLastWriteTime(char *Filename) {
    FILETIME LastWriteTime = {0};

    WIN32_FIND_DATA FindData;
    HANDLE          FindHandle = FindFirstFileA(Filename, &FindData);
    if (FindHandle != INVALID_HANDLE_VALUE) {
        LastWriteTime = FindData.ftLastWriteTime;
        FindClose(FindHandle);
    }

    return (LastWriteTime);
}

GAME_INIT(GameInitStub) {
}

GAME_UPDATE(GameUpdateStub) {
}

gameCode_t LoadGameCode(char *source, char *temp) {
    gameCode_t result = {0};
    result.gameCodeDLLName = temp;
    result.lastDLLWriteTime = GetFileLastWriteTime(source);
    CopyFile(source, temp, FALSE);
    result.gameCodeDLL = LoadLibraryA(temp);
    if (result.gameCodeDLL) {
        result.gameInit = (GameInit_t *)GetProcAddress(result.gameCodeDLL, "GameInit");
        result.gameUpdate = (GameUpdate_t *)GetProcAddress(result.gameCodeDLL, "GameUpdate");
        result.gameShutdown = (GameShutdown_t *)GetProcAddress(result.gameCodeDLL, "GameShutdown");
        result.isValid = result.gameInit && result.gameUpdate && result.gameShutdown;
    } else {
        // TODO(pf): Logging.
        DWORD h = GetLastError();
        UNUSED(h);
        printf("ERR");
    }
    return result;
}

void UnloadGameCode(gameCode_t *gc) {
    if (gc->gameCodeDLL) {
        FreeLibrary(gc->gameCodeDLL);
        gc->gameCodeDLL = 0;
    }

    gc->gameInit = GameInitStub;
    gc->gameUpdate = GameUpdateStub;
}

HI_RES_PERF_FREQ(HiResPerformanceFreq) {
    LARGE_INTEGER countsPerSec;
    QueryPerformanceFrequency(&countsPerSec);
    s64 result = countsPerSec.QuadPart;
    return result;
}

HI_RES_PERF_FREQ(HiResPerformanceQuery) {
    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);
    s64 result = currentTime.QuadPart;
    return result;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void WindowsInitGameCode(char *sourceDLL, char *targetDLL, char *pdbFile, w64_state_t *state) {
    char exeFileName[MAX_PATH];
    /*DWORD sizeOfFilename = */ GetModuleFileNameA(0, exeFileName, sizeof(exeFileName));
    char *onePastLastSlash = exeFileName;
    for (char *scan = exeFileName; *scan; ++scan) {
        if (*scan == '\\') {
            onePastLastSlash = scan + 1;
        }
    }

    char   sourceGameCodeDLLFilename[] = "game.dll";
    char   sourceGameCodeDLLFullPath[MAX_PATH];
    size_t sourceSize = CatStrings(onePastLastSlash - exeFileName, exeFileName,
                                   sizeof(sourceGameCodeDLLFilename) - 1, sourceGameCodeDLLFilename,
                                   sizeof(sourceGameCodeDLLFullPath), sourceGameCodeDLLFullPath);

    char   tempGameCodeDLLFilename[] = "game_temp.dll";
    char   tempGameCodeDLLFullPath[MAX_PATH];
    size_t tempSize = CatStrings(onePastLastSlash - exeFileName, exeFileName,
                                 sizeof(tempGameCodeDLLFilename) - 1, tempGameCodeDLLFilename,
                                 sizeof(tempGameCodeDLLFullPath), tempGameCodeDLLFullPath);

    char   pdbLockFilename[] = "pdb.lock";
    char   pdbLockFileFullPath[MAX_PATH];
    size_t pdbSize = CatStrings(onePastLastSlash - exeFileName, exeFileName,
                                sizeof(pdbLockFilename) - 1, pdbLockFilename,
                                sizeof(pdbLockFileFullPath), pdbLockFileFullPath);
    state->sourceDLL = malloc(sourceSize + 1);
    CopyStrings(sourceGameCodeDLLFullPath, state->sourceDLL, sourceSize);
    state->targetDLL = malloc(tempSize + 1);
    CopyStrings(tempGameCodeDLLFullPath, state->targetDLL, tempSize);
    state->pdbLockFile = malloc(pdbSize + 1);
    CopyStrings(pdbLockFileFullPath, state->pdbLockFile, pdbSize);
}

w64_state_t WindowsInit(u32 wWidth, u32 wHeight, const char *appName) {
    w64_state_t result = {0};
    WNDCLASSEX  wc = {0};
    HINSTANCE   hinstance = GetModuleHandle(NULL);

    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hinstance;
    wc.hIcon = LoadIcon(NULL, IDC_ARROW);
    wc.hIconSm = wc.hIcon;
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = appName;
    wc.cbSize = sizeof(WNDCLASSEX);

    RegisterClassEx(&wc);

    result.windowWidth = wWidth;
    result.windowHeight = wHeight;
    RECT rect = {0};
    rect.top = 50;
    rect.left = 50;
    rect.bottom = 0 + wHeight;
    rect.right = 0 + wWidth;
    DWORD windowStyles = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP;
    AdjustWindowRect(&rect, windowStyles, true);
    result.hwnd = CreateWindowEx(WS_EX_APPWINDOW, appName, appName,
                                 windowStyles,
                                 rect.left, rect.top, rect.right + 8, rect.bottom + 32,
                                 NULL, NULL, hinstance, NULL);
    ShowWindow(result.hwnd, SW_SHOW);
    SetForegroundWindow(result.hwnd);
    SetFocus(result.hwnd);

    WindowsInitGameCode("game.dll", "game_temp.dll", "pdb.lock", &result);

    return result;
}

void ProcessWindowsMessageQueue(platformState_t *state) {
    MSG msg;
    while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
        switch (msg.message) {
        case WM_QUIT: {
            state->isRunning = false;
        } break;

        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP: {
            u32 vkCode = (u32)msg.wParam;
            b32 WasDown = ((msg.lParam & (1 << 30)) != 0);
            b32 isDown = ((msg.lParam & (1 << 31)) == 0);
            if (WasDown != isDown) {
                InputUpdateKey(state->input, vkCode, isDown);
            }

            b32 altKeyWasDown = (msg.lParam & (1 << 29));
            if ((vkCode == VK_F4) && altKeyWasDown) {
                state->isRunning = false;
            }
        } break;

        default: {
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        } break;
        }
    }
}

/*==============================RENDERING==============================*/

static void DrawBufferClear(w64_drawbuffer_t *buffer, v4f color) {

    u32 packedColor = ConvertToPackedU32(color);

    if ((packedColor << 8) == 0)
        memset(buffer->memory, 0, buffer->width * buffer->height * buffer->bytesPerPixel);

    else {
        u32 *p = (u32 *)buffer->memory;
        for (int i = 0; i < buffer->width * buffer->height; ++i)
            *p++ = packedColor;
    }
}

static void DrawBufferLine(w64_drawbuffer_t *buffer, s32 x0, s32 y0, s32 x1, s32 y1,
                           v4f color0, v4f color1) {
    // Clamp into buffer: "clipping ?"
    x0 = ContainS32(x0, 0, buffer->width);
    y0 = ContainS32(y0, 0, buffer->height);
    x1 = ContainS32(x1, 0, buffer->width);
    y1 = ContainS32(y1, 0, buffer->height);

    s32 xMin = MinS32(x0, x1);
    s32 xMax = MaxS32(x0, x1);

    s32 yMin = MinS32(y0, y1);
    s32 yMax = MaxS32(y0, y1);

    s32 steps = MaxS32((yMax - yMin), (xMax - xMin));
    if (steps > 0) {
        f32 dx = (x1 - x0) / (f32)steps;
        f32 dy = (y1 - y0) / (f32)steps;
        f32 stepSize = 1.0f / steps;
        for (s32 step = 0; step < steps; ++step) {
            s32 x = ContainS32((s32)(x0 + (dx * step)), 0, buffer->width);
            s32 y = ContainS32((s32)(y0 + (dy * step)), 0, buffer->height);

            u32 *p = (u32 *)buffer->memory + ((y * buffer->width) + x);
            v4f  lerpC = v4f_lerp(color0, color1, stepSize * step);
            u32  packedColor = ConvertToPackedU32(lerpC);
            *p = packedColor;
        }
    }
}

static w64_rendering_t RenderingInit(w64_state_t *handle) {

    w64_rendering_t result = {0};
    result.backBuffer.width = handle->windowWidth;
    result.backBuffer.height = handle->windowHeight;
    result.backBuffer.bytesPerPixel = 4;
    result.backBuffer.pitch = result.backBuffer.width * result.backBuffer.bytesPerPixel;
    result.backBuffer.memory = malloc(result.backBuffer.width * result.backBuffer.height * result.backBuffer.bytesPerPixel);

    result.backBufferInfo.bmiHeader.biSize = sizeof(result.backBufferInfo.bmiHeader);
    result.backBufferInfo.bmiHeader.biWidth = result.backBuffer.width;
    result.backBufferInfo.bmiHeader.biHeight = -(s32)(result.backBuffer.height);
    result.backBufferInfo.bmiHeader.biPlanes = 1;
    result.backBufferInfo.bmiHeader.biBitCount = (WORD)(result.backBuffer.bytesPerPixel * 8);
    result.backBufferInfo.bmiHeader.biCompression = BI_RGB;

    result.deviceContext = GetDC(handle->hwnd);

    return result;
}

static void RenderingFrame(renderList_t *rl, w64_drawbuffer_t *backBuffer) {

    u8 *base = (u8 *)rl->renderMemory;
    u8 *end = (u8 *)rl->renderMemory + rl->renderMemoryCurrSz;
    for (; base != end;) {

        renderCommand_t *rc = (renderCommand_t *)base;
        size_t           rc_size = sizeof(renderCommand_t);
        switch (rc->type) {

        case rcClearColor: {
            rcClearColor_t *cmd = (rcClearColor_t *)base;
            DrawBufferClear(backBuffer, cmd->color);
            rc_size = sizeof(rcClearColor_t);
        } break;

        case rcLine: {
            rcLine_t *cmd = (rcLine_t *)base;
            s32       x0 = (s32)(cmd->p0.x * rl->metersToPixels);
            s32       y0 = (s32)(cmd->p0.y * rl->metersToPixels);
            s32       x1 = (s32)(cmd->p1.x * rl->metersToPixels);
            s32       y1 = (s32)(cmd->p1.y * rl->metersToPixels);
            // flip y.
            y0 = rl->windowHeight - y0;
            y1 = rl->windowHeight - y1;
            DrawBufferLine(backBuffer, x0, y0, x1, y1, cmd->col0, cmd->col1);
            rc_size = sizeof(rcLine_t);
        } break;

        default:
            break;
        }

        base = ((u8 *)base) + rc_size;
    }
}

static void RenderingFlip(w64_rendering_t *rs) {
    StretchDIBits(rs->deviceContext, 0, 0,
                  rs->backBuffer.width, rs->backBuffer.height,
                  0, 0, rs->backBuffer.width, rs->backBuffer.height,
                  rs->backBuffer.memory, &rs->backBufferInfo,
                  DIB_RGB_COLORS, SRCCOPY);

    if (rs->vsync) {
        Assert(DwmFlush() == S_OK);
    }
}

int main(int argc, char **argv) {

    size_t renderMemorySize = MB(1);
    void  *renderMemory = malloc(renderMemorySize);

    size_t appMemorySize = GB(1);
    void  *appMemory = malloc(appMemorySize);

    platformState_t ps = {0};
    ps.memorySize = appMemorySize;
    ps.memory = appMemory;
    ps.isRunning = true;

    input_t input = {0};
    ps.input = &input;

    char       *appName = "Minecraft";
    w64_state_t winState = WindowsInit(1280, 720, appName);
    ps.platformHandle = &winState;
    ps.performanceFreq = HiResPerformanceFreq();
    ps.PerfQuery = &HiResPerformanceQuery;

    f64 totalTime = 0.0f;
    f64 prevTotalTime = 0.0f;
    f64 deltaTime = 0.0f;

    gameCode_t gc = LoadGameCode(winState.sourceDLL, winState.targetDLL);
    Assert(gc.isValid);

    w64_rendering_t rendering = RenderingInit(&winState);

    renderList_t renderList = {0};
    renderList.windowWidth = winState.windowWidth;
    renderList.windowHeight = winState.windowHeight;
    renderList.renderMemory = renderMemory;
    renderList.renderMemoryMaxSz = renderMemorySize;

    renderList.aspectRatio = (f32)renderList.windowWidth / renderList.windowHeight;
    renderList.metersToPixels = 40.0f;
    ps.renderList = &renderList;

    gc.gameInit(&ps, false);
    while (ps.isRunning) {
        s64 startTime = HiResPerformanceQuery();
        ProcessWindowsMessageQueue(&ps);

        FILETIME newDLLWriteTime = GetFileLastWriteTime(winState.sourceDLL);
        if (CompareFileTime(&newDLLWriteTime, &gc.lastDLLWriteTime) != 0) {
            DWORD attributes = GetFileAttributes(winState.pdbLockFile);
            if (attributes == INVALID_FILE_ATTRIBUTES) {
                UnloadGameCode(&gc);
                gc = LoadGameCode(winState.sourceDLL, winState.targetDLL);
                if (gc.isValid) {
                    gc.gameInit(&ps, true);
                } else {
                    ps.isRunning = false;
                }
            } else {
                printf("Lock file present, waiting with loading DLL..\n");
            }
        }

        if (gc.gameUpdate) {
            gc.gameUpdate(&ps);
        }

        RenderingFrame(&renderList, &rendering.backBuffer);
        RenderingFlip(&rendering);
        renderList.renderMemoryCurrSz = 0;

        InputUpdate(ps.input);
        s64 endTime = HiResPerformanceQuery();
        deltaTime = (f32)MaxF64(((endTime - startTime) / (f64)ps.performanceFreq), 0);
        totalTime += deltaTime;
        ps.deltaTime = (f32)deltaTime;

        if ((totalTime - prevTotalTime) >= 0.5f) {
            printf("FPS: %ld, Delta(ms): %lf Elapsed Time: %ld\n", (long)(1.0f / deltaTime), deltaTime * 1000.0f, (long)totalTime);
            prevTotalTime = totalTime;
        }
    }
    gc.gameShutdown(&ps);
    UnloadGameCode(&gc);
    return 0;
}