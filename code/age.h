/* ========================================================================
   Creator: Grimleik $
   ========================================================================*/
#if !defined(AGE_H)
#define AGE_H

/*==============================INCLUDES==============================*/

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>
#include <xmmintrin.h>
#include <vector>

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
#define HALF_PI PI * 0.5f
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

struct v2f {
    union {
        struct {
            f32 x, y;
        };
        f32 e[2];
    };
};

inline v2f operator/(v2f a, f32 s) {
    return v2f{a.x / s, a.y / s};
}

inline f32 length(v2f a) {
    return (f32)sqrt(a.x * a.x + a.y * a.y);
}

inline v2f normalize(v2f a) {
    return a / length(a);
}

inline f32 dot(v2f a, v2f b) {
    return a.x * b.x + a.y * b.y;
}

struct v3f {
    union {
        struct {
            f32 x, y, z;
        };

        struct {
            f32 r, g, b;
        };
        f32 e[3];
        v2f xy;
    };
};

inline v3f operator+(v3f a, v3f b) {
    return v3f{a.x + b.x, a.y + b.y, a.z + b.z};
}

inline v3f operator-(v3f a, v3f b) {
    return v3f{a.x - b.x, a.y - b.y, a.z - b.z};
}

inline v3f operator*(f32 s, v3f a) {
    return v3f{a.x * s, a.y * s, a.z * s};
}

inline v3f operator*(v3f a, f32 s) {
    return v3f{a.x * s, a.y * s, a.z * s};
}
inline v3f cross(v3f a, v3f b) {
    return v3f{a.y * b.z - a.z * b.y,
               a.z * b.x - a.x - b.z,
               a.x * b.y - a.y * b.x};
}

struct v4f {
    union {
        struct {
            f32 x, y, z, w;
        };
        struct {
            f32 r, g, b, a;
        };
        f32 e[4];
    };
};

inline v4f operator+(const v4f a, f32 s) {
    return v4f{a.x + s, a.y + s, a.z + s, a.w + s};
}

inline v4f operator+(f32 s, const v4f a) {
    return v4f{a.x + s, a.y + s, a.z + s, a.w + s};
}

inline v4f operator+(const v4f a, v4f b) {
    return v4f{a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
}

inline v4f operator*(const v4f a, const f32 s) {
    return v4f{a.x * s, a.y * s, a.z * s, a.w * s};
}

inline v4f operator*(const f32 s, const v4f a) {
#if 0
    v4f result;
    __m128 scalar = _mm_set1_ps(s);
    __m128 vec = _mm_load_ps(a.e);
    __m128 res = _mm_mul_ps(scalar, vec);
    _mm_storeu_ps(result.e, res);
    return result;
#else
    return v4f{a.x * s, a.y * s, a.z * s, a.w * s};
#endif
}

inline v4f lerp(const v4f a, const v4f b, const f32 t) {
    return (1 - t) * a + b * t;
}

#define ageCOLOR_RED \
    v4f { 1.0f, 0.0f, 0.0f, 1.0f }
#define ageCOLOR_GREEN \
    v4f { 0.0f, 1.0f, 0.0f, 1.0f }
#define ageCOLOR_BLACK \
    v4f { 0.0f }
#define ageCOLOR_BLUE \
    v4f { 0.0f, 0.0f, 1.0f, 1.0f }
#define ageCOLOR_WHITE \
    v4f { 1.0f, 1.0f, 1.0f, 1.0f }

inline u32 ConvertToPackedU32(const v4f vf) {
#if 1
    u32 r = (u32)(255.0f * vf.r);
    u32 g = (u32)(255.0f * vf.g);
    u32 b = (u32)(255.0f * vf.b);
    u32 a = (u32)(255.0f * vf.a);
    return a << 24 | r << 16 | g << 8 | b;
#else
    return ((u32)((u32)(255.0f * vf.a) << 24 |
                  (u32)(255.0f * vf.r) << 16 |
                  (u32)(255.0f * vf.g) << 8 |
                  (u32)(255.0f * vf.b)));
#endif
}

inline f64 Max(f64 a, f64 b) {
    return a > b ? a : b;
}

inline f32 Max(f32 a, f32 b) {
    return a > b ? a : b;
}

inline f32 Min(f32 a, f32 b) {
    return a < b ? a : b;
}

inline s32 Max(s32 a, s32 b) {
    return a > b ? a : b;
}

inline s32 Min(s32 a, s32 b) {
    return a < b ? a : b;
}

inline s32 Contain(s32 val, s32 minVal, s32 maxVal) {
    return Max(Min(val, maxVal), minVal);
}

inline f32 Contain(f32 val, f32 minVal, f32 maxVal) {
    return Max(Min(val, maxVal), minVal);
}

inline s32 Wrap(s32 x, s32 xMax) {
    s32 result = (x + xMax) % xMax;
    return result;
}

inline f32 Clamp(f32 x, f32 xMin, f32 xMax) {
    f32 result = x > xMax ? xMax : (x < xMin ? xMin : x);
    return result;
}

inline f32 Square(f32 value) {
    f32 result = value * value;
    return result;
}

inline void SetSeed(u32 seed) {
    srand(seed);
}

inline f32 Random01() {
    return rand() / (f32)(RAND_MAX);
}

inline f32 Random(f32 min, f32 max) {
    return min + (Random01() * (max - min));
}

enum RC_TYPE {
    rcClearColor,
    rcLine,
    rcTriangleOutline,
};

struct renderCommand_t {
    RC_TYPE type;
};

struct rcClearColor_t {
    renderCommand_t base;
    v4f             color;
};

struct rcLine_t {
    renderCommand_t base;
    v2f             p0;
    v2f             p1;
    v4f             col0;
    v4f             col1;
};

struct rcTriangleOutline_t {
    renderCommand_t base;
    v3f             vertices[3];
    v4f             vert_col[3];
};

struct renderList_t {
    u32    windowWidth;
    u32    windowHeight;
    f32    aspectRatio;
    f32    metersToPixels;
    void  *renderMemory;
    size_t renderMemoryMaxSz;
    size_t renderMemoryCurrSz;
};

void *PushRenderCommand_(renderList_t *rl, RC_TYPE type, size_t sz);
#define PushRenderCommand(rl, TYPE) (TYPE##_t *)PushRenderCommand_(rl, TYPE, sizeof(TYPE##_t))

enum FRAME {
    FRAME_CURRENT = 0,
    FRAME_PREVIOUS = 1,
    FRAME_COUNT,
};

enum AXIS {
    AXIS_MOUSE,
    AXIS_MOUSE_WHEEL,
    AXIS_GP_LEFT,
    AXIS_GP_RIGHT,
    AXIS_MAX_STATES,
};

// NOTE(pf): These are based on windows virtual keys, other platforms might have
// to do a mapping function in the input handling.
enum KEY {
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
};
struct input_t {

    struct keyState_t {
        b32 isDown;
    };

    struct axisState_t {
        f32 x, y;
    };

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
};

void InputUpdate(input_t *);

void InputUpdateKey(input_t *, u32, b32);
void InputUpdateAxis(input_t *, u8, f32, f32);

b32 InputKeyPressed(input_t *, u32);
b32 InputKeyDown(input_t *, u32);
b32 InputKeyRelease(input_t *, u32);

struct platformState_t {
    input_t      *input;
    renderList_t *renderList;
    void         *memory;
    MemoryMarker  memorySize;
    f32           deltaTime;
    b32           isRunning;
    void         *platformHandle;
};

#define GAME_INIT(name) void name(platformState_t *platformState, b32 reloaded)
typedef GAME_INIT(GameInit_t);

#define GAME_UPDATE(name) void name(platformState_t *platformState)
typedef GAME_UPDATE(GameUpdate_t);

#define GAME_SHUTDOWN(name) void name(platformState_t *platformState)
typedef GAME_SHUTDOWN(GameShutdown_t);

#endif
