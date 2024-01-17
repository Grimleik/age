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
#include <memory>

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

#define KB(Value) ((Value) * 1024LL)
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

inline v3f operator/(v3f a, f32 s) {
    return v3f{a.x / s, a.y / s, a.z / s};
}

inline f32 length(v3f a) {
    return (f32)sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

inline v3f normalize(v3f a) {
    return a / length(a);
}

inline f32 dot(v3f a, v3f b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

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
    v4f() : x(0), y(0), z(0), w(0){};
    v4f(f32 f) : x(f), y(f), z(f), w(f){};
    v4f(f32 _x, f32 _y, f32 _z, f32 _w) : x(_x), y(_y), z(_z), w(_w){};
    v4f(v3f v, f32 f) : x(v.x), y(v.y), z(v.z), w(f){};
    union {
        struct {
            f32 x, y, z, w;
        };
        struct {
            f32 r, g, b, a;
        };
        f32 e[4];
        v3f xyz;
    };
};

inline v4f operator+(const v4f a, f32 s) {
    return v4f{a.x + s, a.y + s, a.z + s, a.w + s};
}

inline void operator+=(v4f &a, const v4f b) {
    a.x += b.x;
    a.y += b.y;
    a.z += b.z;
    a.w += b.w;
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

inline void operator*=(v4f &a, const f32 b) {
    a.x *= b;
    a.y *= b;
    a.z *= b;
    a.w *= b;
}

inline v4f operator/(v4f a, f32 s) {
    return v4f{a.x / s, a.y / s, a.z / s, a.w / s};
}

inline void operator/=(v4f &a, const f32 b) {
    a.x /= b;
    a.y /= b;
    a.z /= b;
    a.w /= b;
}

inline f32 length(v4f a) {
    return (f32)sqrt(a.x * a.x + a.y * a.y + a.z * a.z + a.w * a.w);
}

inline v4f normalize(v4f a) {
    return a / length(a);
}

inline v4f hadamard(v4f a, v4f b) {
    return {a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w};
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

struct mat4x4 {
    mat4x4() {
        r[0] = v4f{};
        r[1] = v4f{};
        r[2] = v4f{};
        r[3] = v4f{};
    }
    union {
        f32 e[4][4];
        v4f r[4];
    };
};

inline mat4x4 Mat4Identity() {
    mat4x4 result = {};
    result.e[0][0] = 1.0f;
    result.e[1][1] = 1.0f;
    result.e[2][2] = 1.0f;
    result.e[3][3] = 1.0f;
    return result;
}

inline mat4x4 Mat4RotationX(f32 theta) {
    mat4x4 result = {};
    result.e[0][0] = 1.0f;
    result.e[1][1] = (f32)cos(theta);
    result.e[1][2] = -(f32)sin(theta);
    result.e[2][1] = (f32)sin(theta);
    result.e[2][2] = (f32)cos(theta);

    return result;
}

inline mat4x4 Mat4RotationY(f32 theta) {
    mat4x4 result = {};
    result.e[0][0] = (f32)cos(theta);
    result.e[0][2] = (f32)sin(theta);
    result.e[1][1] = 1.0f;
    result.e[2][0] = -(f32)sin(theta);
    result.e[2][2] = (f32)cos(theta);

    return result;
}

inline mat4x4 Mat4RotationZ(f32 theta) {
    mat4x4 result = {};
    result.e[0][0] = (f32)cos(theta);
    result.e[0][1] = -(f32)sin(theta);
    result.e[1][0] = (f32)sin(theta);
    result.e[1][1] = (f32)cos(theta);
    result.e[2][2] = 1.0f;
    return result;
}

inline mat4x4 Mat4Projection(f32 a, f32 fov, f32 near, f32 far) {
    f32    fovRad = 1.0f / tanf(fov * 0.5f / 180.0f * PI);
    mat4x4 result;
    result.e[0][0] = a * fovRad;
    result.e[1][1] = fovRad;
    result.e[2][2] = far / (far - near);
    result.e[3][2] = (-far * near) / (far - near);
    result.e[2][3] = 1.0f;
    result.e[3][3] = 0.0f;
    return result;
}

inline v4f operator*(const v4f &v, const mat4x4 &mat) {
    v4f result;
    result.x = v.x * mat.e[0][0] + v.y * mat.e[1][0] + v.z * mat.e[2][0] + v.w * mat.e[3][0];
    result.y = v.x * mat.e[0][1] + v.y * mat.e[1][1] + v.z * mat.e[2][1] + v.w * mat.e[3][1];
    result.z = v.x * mat.e[0][2] + v.y * mat.e[1][2] + v.z * mat.e[2][2] + v.w * mat.e[3][2];
    result.w = v.x * mat.e[0][3] + v.y * mat.e[1][3] + v.z * mat.e[2][3] + v.w * mat.e[3][3];
    return result;
}

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
    rcGroup,
    rcClearColor,
    rcLine,
    rcTriangle,
    rcTriangleOutline,
};

struct renderCommand_t {
    RC_TYPE type;
};

struct rcGroup_t {
    renderCommand_t base;
    mat4x4          projection;
    mat4x4          view;
    mat4x4          model;
    v4f             translation;
    size_t          commandCount; // nr of following commands to apply these settings to.
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

struct rcTriangle_t {
    renderCommand_t base;
    v3f             vertices[3];
    v4f             vert_col[3];
    b32             interpolate;
};

struct rcTriangleOutline_t {
    renderCommand_t base;
    v3f             vertices[3];
    v4f             vert_col[3];
};

enum RENDER_MODE {
    Software,
    OpenGL,
    // TODO:
    // Vulkan,
    // DX11,
    // DX12,
};

struct renderList_t {
    u32         windowWidth;
    u32         windowHeight;
    f32         aspectRatio;
    f32         metersToPixels;
    RENDER_MODE mode;
    void       *renderMemory;
    size_t      renderMemoryMaxSz;
    size_t      renderMemoryCurrSz;
    size_t      commandCount;
    size_t      dbgOpenGroups;

    void Reset() {
        renderMemoryCurrSz = 0;
        commandCount = 0;
    }
};

void *PushRenderCommand_(renderList_t *rl, RC_TYPE type, size_t sz);
#define PushRenderCommand(rl, TYPE) (TYPE##_t *)PushRenderCommand_(rl, TYPE, sizeof(TYPE##_t))

rcGroup_t *BeginRenderGroup(renderList_t *rl, mat4x4 proj, mat4x4 view, mat4x4 model);
void       EndRenderGroup(rcGroup_t *group, renderList_t *rl);
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

class MemoryStack {
  public:
    MemoryStack();
    ~MemoryStack();

    void Initialize(void *mem, MemoryMarker size);

    void ResetToMarker(MemoryMarker marker);

    void ZeroOutToMarker(MemoryMarker marker);

    void *Allocate(MemoryMarker sizeInBytes);

    template <typename T>
    T *Allocate(u32 count = 1);

    template <typename T>
    T *AllocateAndInitialize(u32 count = 1);

    template <typename T>
    void ResetToObject(T *obj);

  private:
    void        *memory;
    MemoryMarker totalSize;
    MemoryMarker currentSize;
};

template <typename T>
inline T *MemoryStack::Allocate(u32 count) {
    MemoryMarker sizeInBytes = sizeof(T) * count;
    Assert(currentSize + sizeInBytes <= totalSize);
    void *result = (u8 *)(memory) + currentSize;
    currentSize += sizeInBytes;
    return (T *)result;
}

template <typename T>
inline T *MemoryStack::AllocateAndInitialize(u32 count) {
    MemoryMarker sizeInBytes = sizeof(T) * count;
    Assert(currentSize + sizeInBytes <= totalSize);
    void *pMem = (u8 *)(memory) + currentSize;
    currentSize += sizeInBytes;
    T *result = (T *)pMem;
    *result = {};
    return result;
}

template <typename T>
inline void MemoryStack::ResetToObject(T *obj) {
    Memorymarker mmReset = (u8 *)obj - (u8 *)memory;
    ResetToMarker(mmReset);
}

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
