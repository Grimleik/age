/* ========================================================================
   Creator: Grimleik $
   ========================================================================*/
#include "age.h"
#include "w64_main.h"
#include "w64_renderer.h"
#include <iostream>
#include <fstream>
#include <sstream>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

void CopyStrings(char *source, char *dest, size_t size)
{
	for (int i = 0; i < size; ++i)
		*dest++ = *source++;
	*dest = '\0';
}

size_t CatStrings(size_t SourceACount, char *SourceA,
				  size_t SourceBCount, char *SourceB,
				  size_t DestCount, char *Dest)
{
	for (int Index = 0;
		 Index < SourceACount;
		 ++Index)
	{
		*Dest++ = *SourceA++;
	}

	for (int Index = 0;
		 Index < SourceBCount;
		 ++Index)
	{
		*Dest++ = *SourceB++;
	}

	*Dest++ = 0;
	return SourceACount + SourceBCount;
}

FILETIME GetFileLastWriteTime(char *Filename)
{
	FILETIME LastWriteTime = {0};

	WIN32_FIND_DATA FindData;
	HANDLE FindHandle = FindFirstFileA(Filename, &FindData);
	if (FindHandle != INVALID_HANDLE_VALUE)
	{
		LastWriteTime = FindData.ftLastWriteTime;
		FindClose(FindHandle);
	}

	return (LastWriteTime);
}

GAME_INIT(GameInitStub)
{
}

GAME_UPDATE(GameUpdateStub)
{
}

gameCode_t LoadGameCode(char *source, char *temp)
{
	gameCode_t result = {0};
	result.gameCodeDLLName = temp;
	result.lastDLLWriteTime = GetFileLastWriteTime(source);
	CopyFile(source, temp, FALSE);
	result.gameCodeDLL = LoadLibraryA(temp);
	if (result.gameCodeDLL)
	{
		result.gameInit = (GameInit_t *)GetProcAddress(result.gameCodeDLL, "GameInit");
		result.gameUpdate = (GameUpdate_t *)GetProcAddress(result.gameCodeDLL, "GameUpdate");
		result.gameShutdown = (GameShutdown_t *)GetProcAddress(result.gameCodeDLL, "GameShutdown");
		result.isValid = result.gameInit && result.gameUpdate && result.gameShutdown;
	}
	else
	{
		// TODO(pf): Logging.
		DWORD h = GetLastError();
		UNUSED(h);
		printf("ERR");
	}
	return result;
}

void UnloadGameCode(gameCode_t *gc)
{
	if (gc->gameCodeDLL)
	{
		FreeLibrary(gc->gameCodeDLL);
		gc->gameCodeDLL = 0;
	}

	gc->gameInit = GameInitStub;
	gc->gameUpdate = GameUpdateStub;
}

s64 HiResPerformanceFreq()
{
	LARGE_INTEGER countsPerSec;
	QueryPerformanceFrequency(&countsPerSec);
	s64 result = countsPerSec.QuadPart;
	return result;
}

s64 HiResPerformanceQuery()
{
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	s64 result = currentTime.QuadPart;
	return result;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void WindowsInitGameCode(char *sourceDLL, char *targetDLL, char *pdbFile, w64State_t *state)
{
	char exeFileName[MAX_PATH];
	/*DWORD sizeOfFilename = */ GetModuleFileNameA(0, exeFileName, sizeof(exeFileName));
	char *onePastLastSlash = exeFileName;
	for (char *scan = exeFileName; *scan; ++scan)
	{
		if (*scan == '\\')
		{
			onePastLastSlash = scan + 1;
		}
	}

	char sourceGameCodeDLLFilename[] = "game.dll";
	char sourceGameCodeDLLFullPath[MAX_PATH];
	size_t sourceSize = CatStrings(onePastLastSlash - exeFileName, exeFileName,
								   sizeof(sourceGameCodeDLLFilename) - 1, sourceGameCodeDLLFilename,
								   sizeof(sourceGameCodeDLLFullPath), sourceGameCodeDLLFullPath);

	char tempGameCodeDLLFilename[] = "game_temp.dll";
	char tempGameCodeDLLFullPath[MAX_PATH];
	size_t tempSize = CatStrings(onePastLastSlash - exeFileName, exeFileName,
								 sizeof(tempGameCodeDLLFilename) - 1, tempGameCodeDLLFilename,
								 sizeof(tempGameCodeDLLFullPath), tempGameCodeDLLFullPath);

	char pdbLockFilename[] = "pdb.lock";
	char pdbLockFileFullPath[MAX_PATH];
	size_t pdbSize = CatStrings(onePastLastSlash - exeFileName, exeFileName,
								sizeof(pdbLockFilename) - 1, pdbLockFilename,
								sizeof(pdbLockFileFullPath), pdbLockFileFullPath);
	state->sourceDLL = (char *)malloc(sourceSize + 1);
	CopyStrings(sourceGameCodeDLLFullPath, state->sourceDLL, sourceSize);
	state->targetDLL = (char *)malloc(tempSize + 1);
	CopyStrings(tempGameCodeDLLFullPath, state->targetDLL, tempSize);
	state->pdbLockFile = (char *)malloc(pdbSize + 1);
	CopyStrings(pdbLockFileFullPath, state->pdbLockFile, pdbSize);
}

w64State_t WindowsInit(u32 wWidth, u32 wHeight, const char *appName)
{
	w64State_t result = {0};
	WNDCLASSEX wc = {0};
	HINSTANCE hinstance = GetModuleHandle(NULL);

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

void ProcessWindowsMessageQueue(platformState_t *state)
{
	MSG msg;
	while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
		switch (msg.message)
		{
		case WM_QUIT:
		{
			state->isRunning = false;
		}
		break;

		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		case WM_KEYDOWN:
		case WM_KEYUP:
		{
			u32 vkCode = (u32)msg.wParam;
			b32 WasDown = ((msg.lParam & (1 << 30)) != 0);
			b32 isDown = ((msg.lParam & (1 << 31)) == 0);
			if (WasDown != isDown)
			{
				InputUpdateKey(state->input, vkCode, isDown);
			}

			b32 altKeyWasDown = (msg.lParam & (1 << 29));
			if ((vkCode == VK_F4) && altKeyWasDown)
			{
				state->isRunning = false;
			}
		}
		break;

		default:
		{
			TranslateMessage(&msg);
			DispatchMessageA(&msg);
		}
		break;
		}
	}
}

int main(int argc, char **argv)
{
	size_t renderMemorySize = MB(10);
	void *renderMemory = malloc(renderMemorySize);

	size_t appMemorySize = GB(1);
	void *appMemory = malloc(appMemorySize);

	platformState_t ps = {0};
	ps.memorySize = appMemorySize;
	ps.memory = appMemory;
	ps.isRunning = true;

	input_t input = {0};
	ps.input = &input;

	char *appName = "App";
	s32 screenW = 1280;
	s32 screenH = 720;

	w64State_t winState = WindowsInit(screenW, screenH, appName);
	ps.platformHandle = &winState;

	f64 totalTime = 0.0f;
	f64 prevTotalTime = 0.0f;
	f64 deltaTime = 0.0f;

	renderList_t renderList = {0};
	renderList.windowWidth = winState.windowWidth;
	renderList.windowHeight = winState.windowHeight;
	renderList.renderMemory = renderMemory;
	renderList.renderMemoryMaxSz = renderMemorySize;

	renderList.aspectRatio = (f32)renderList.windowWidth / renderList.windowHeight;
	renderList.metersToPixels = 40.0f;
	ps.renderList = &renderList;

	gameCode_t gc = LoadGameCode(winState.sourceDLL, winState.targetDLL);
	Assert(gc.isValid);
	gc.gameInit(&ps, false);

	// TODO: Dynamic selection of render system ?
	std::unique_ptr<IW64Renderer> renderer = IW64Renderer::Create(ps.renderList->mode, winState);
	if (!renderer->Init(winState))
	{
		Assert(false);
		return 0;
	}

	while (ps.isRunning)
	{
		s64 startTime = HiResPerformanceQuery();
		ProcessWindowsMessageQueue(&ps);

		FILETIME newDLLWriteTime = GetFileLastWriteTime(winState.sourceDLL);
		if (CompareFileTime(&newDLLWriteTime, &gc.lastDLLWriteTime) != 0)
		{
			DWORD attributes = GetFileAttributes(winState.pdbLockFile);
			if (attributes == INVALID_FILE_ATTRIBUTES)
			{
				UnloadGameCode(&gc);
				gc = LoadGameCode(winState.sourceDLL, winState.targetDLL);
				if (gc.isValid)
				{
					gc.gameInit(&ps, true);
				}
				else
				{
					ps.isRunning = false;
				}
			}
			else
			{
				printf("Lock file present, waiting with loading DLL..\n");
			}
		}

		if (gc.gameUpdate)
		{
			gc.gameUpdate(&ps);
		}

		Assert(renderList.dbgOpenGroups == 0);
		renderer->Render(renderList);
		renderList.Reset();

		InputUpdate(ps.input);
		s64 endTime = HiResPerformanceQuery();
		deltaTime = (f32)Max(((endTime - startTime) / (f64)HiResPerformanceFreq()), (s64)0);
		totalTime += deltaTime;
		ps.deltaTime = (f32)deltaTime;

		if ((totalTime - prevTotalTime) >= 0.5f)
		{
			printf("FPS: %ld, Delta(ms): %lf Elapsed Time: %ld\n", (long)(1.0f / deltaTime), deltaTime * 1000.0f, (long)totalTime);
			prevTotalTime = totalTime;
		}
		// ps.isRunning = false;
	}
	gc.gameShutdown(&ps);
	UnloadGameCode(&gc);
	return 0;
}