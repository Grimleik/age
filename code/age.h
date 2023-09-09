#if !defined(AGE_H)
#define AGE_H

/*==============================INCLUDES==============================*/

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

/*==============================TYPES==============================*/

typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float  f32;
typedef double f64;

typedef s32 b32;

typedef size_t MemoryMarker;

/*==============================MACROS==============================*/

#define UNUSED(x) x

#define PI 3.14159265359F
#define RAD2DEG 180.F / PI

#define KB(Value) ((Value)*1024LL)
#define MB(Value) (KB(Value) * 1024LL)
#define GB(Value) (MB(Value) * 1024LL)
#define TB(Value) (GB(Value) * 1024LL)
#define ARRAY_COUNT(x) (sizeof(x) / sizeof(x[0]))

#define NOT_YET_IMPLEMENTED(x) error // x

#ifdef _DEBUG
#define Assert(x)       \
    if (!(x)) {         \
        __debugbreak(); \
    }
#define InvalidCodePath Assert(false)

#else
#define InvalidCodePath
#define Assert(x)

#endif
inline f64 MaxF64(f64 a, f64 b) {
    return a > b ? a : b;
}

inline f32 MaxF32(f32 a, f32 b) {
    return a > b ? a : b;
}

inline f32 MinF32(f32 a, f32 b) {
    return a < b ? a : b;
}

inline s32 MaxS32(s32 a, s32 b) {
    return a > b ? a : b;
}

inline s32 MinS32(s32 a, s32 b) {
    return a < b ? a : b;
}

#define Max(X, Y) _Generic((X), \
    f32: MaxF32,                \
    s32: MaxS32)((X), (Y))

#define Min(X, Y) _Generic((X), \
    f32: MinF32,                \
    s32: MinS32)(X, Y)

inline f32 ContainF32(f32 val, f32 minVal, f32 maxVal) {
    return MaxF32(MinF32(val, maxVal), minVal);
}

inline s32 WrapS32(s32 x, s32 xMax) {
    s32 result = (x + xMax) % xMax;
    return result;
}

inline f32 ClampF32(f32 x, f32 xMin, f32 xMax) {
    f32 result = x > xMax ? xMax : (x < xMin ? xMin : x);
    return result;
}

inline f32 SquareF32(f32 value) {
    f32 result = value * value;
    return result;
}

inline void SetSeed(u32 seed) {
    srand(seed);
}

inline f32 Random01() {
    return rand() / (f32)(RAND_MAX);
}

inline f32 RandomF32(f32 min, f32 max) {
    return min + (Random01() * (max - min));
}

typedef struct renderList_t {
    u32 windowWidth;
    u32 windowHeight;
    f32 aspectRatio;
    f32 metersToPixels;
} renderList_t;

typedef enum FRAME {
    FRAME_CURRENT = 0,
    FRAME_PREVIOUS = 1,
    FRAME_COUNT,
} FRAME;

typedef enum AXIS {
    AXIS_MOUSE,
    AXIS_MOUSE_WHEEL,
    AXIS_GP_LEFT,
    AXIS_GP_RIGHT,
    AXIS_MAX_STATES,
} AXIS;

// NOTE(pf): These are based on windows virtual keys, other platforms might have
// to do a mapping function in the input handling.
typedef enum KEY {
    KEY_M1 = 1,
    KEY_M2 = 2,
    KEY_ARROW_LEFT = 0x25,
    KEY_ARROW_UP = 0x26,
    KEY_ARROW_RIGHT = 0x27,
    KEY_ARROW_DOWN = 0x28,
    KEY_ESCAPE = 0x1B,
    KEY_SPACE = 0x20,
    KEY_A = 'A',
    KEY_B = 'B',
    KEY_C = 'C',
    KEY_D = 'D',
    KEY_E = 'E',
    KEY_F = 'F',
    KEY_G = 'G',
    KEY_H = 'H',
    KEY_I = 'I',
    KEY_J = 'J',
    KEY_K = 'K',
    KEY_L = 'L',
    KEY_M = 'M',
    KEY_N = 'N',
    KEY_O = 'O',
    KEY_P = 'P',
    KEY_Q = 'Q',
    KEY_R = 'R',
    KEY_S = 'S',
    KEY_T = 'T',
    KEY_U = 'U',
    KEY_V = 'V',
    KEY_W = 'W',
    KEY_X = 'X',
    KEY_Y = 'Y',
    KEY_Z = 'Z',
    KEY_a = 'a',
    KEY_b = 'b',
    KEY_c = 'c',
    KEY_d = 'd',
    KEY_e = 'e',
    KEY_f = 'f',
    KEY_g = 'g',
    KEY_h = 'h',
    KEY_i = 'i',
    KEY_j = 'j',
    KEY_k = 'k',
    KEY_l = 'l',
    KEY_m = 'm',
    KEY_n = 'n',
    KEY_o = 'o',
    KEY_p = 'p',
    KEY_q = 'q',
    KEY_r = 'r',
    KEY_s = 's',
    KEY_t = 't',
    KEY_u = 'u',
    KEY_v = 'v',
    KEY_w = 'w',
    KEY_x = 'x',
    KEY_y = 'y',
    KEY_z = 'z',
    KEY_K0 = '0',
    KEY_K1 = '1',
    KEY_K2 = '2',
    KEY_K3 = '3',
    KEY_K4 = '4',
    KEY_K5 = '5',
    KEY_K6 = '6',
    KEY_K7 = '7',
    KEY_K8 = '8',
    KEY_K9 = '9',
    KEY_MAX_STATES = 0xFF
} KEY;

typedef struct keyState_t {
    b32 isDown;
} keyState_t;

typedef struct axisState_t {
    f32 x, y;
} axisState_t;

typedef struct input_t {

    // void Update();
    // void UpdateKey(u32 keyCode, b32 keyState);
    // void UpdateAxis(u8 axisCode, f32 x, f32 y);

    // b32 KeyPressed(u32 key, FRAME f = FRAME::CURRENT);
    // b32 KeyDown(u32 key, FRAME f = FRAME::CURRENT);
    // b32 KeyReleased(u32 key, FRAME f = FRAME::CURRENT);

    // f32 AxisX(u32 axis, FRAME f = FRAME::CURRENT);
    // f32 AxisY(u32 axis, FRAME f = FRAME::CURRENT);
    // v2f Axis(u32 axis, FRAME f = FRAME::CURRENT);

    keyState_t  keyStates[FRAME_COUNT][KEY_MAX_STATES];
    axisState_t axisStates[FRAME_COUNT][AXIS_MAX_STATES];
    u8          activeFrame;
} input_t;

void InputUpdate(input_t *);

void InputUpdateKey(input_t *, u32, b32);
void InputUpdateAxis(input_t *, u8, f32, f32);

b32 InputKeyPressed(input_t *, u32);
b32 InputKeyDown(input_t *, u32);
b32 InputKeyRelease(input_t *, u32);

typedef struct platformState_t platformState_t;
#define GAME_INIT(name) void name(platformState_t *platformState, b32 reloaded)
typedef GAME_INIT(GameInit_t);

#define GAME_UPDATE(name) void name(platformState_t *platformState)
typedef GAME_UPDATE(GameUpdate_t);

#define GAME_SHUTDOWN(name) void name(platformState_t *platformState)
typedef GAME_SHUTDOWN(GameShutdown_t);

#define HI_RES_PERF_FREQ(name) s64 name()
typedef HI_RES_PERF_FREQ(HiResPerformanceFreq_t);

#define HI_RES_PERF_QUERY(name) s64 name()
typedef HI_RES_PERF_QUERY(HiResPerformanceQuery_t);

typedef struct platformState_t {
    HiResPerformanceQuery_t *PerfQuery;
    input_t                 *input;
    renderList_t            *renderList;
    void                    *memory;
    MemoryMarker             memorySize;
    s64                      performanceFreq;
    f32                      deltaTime;
    b32                      isRunning;
    void                    *platformHandle;
} platformState_t;

#endif
