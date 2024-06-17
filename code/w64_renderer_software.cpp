/* ========================================================================
   Creator: Grimleik $
   ========================================================================*/
#include "w64_renderer_software.h"
#include <iostream>

#include <dwmapi.h>
#pragma comment(lib, "Dwmapi.lib")

W64RendererSoftware::W64RendererSoftware()
{
}

W64RendererSoftware::~W64RendererSoftware()
{
}

bool W64RendererSoftware::Init(w64State_t &state)
{

    backBuffer.width = state.windowWidth;
    backBuffer.height = state.windowHeight;
    backBuffer.bytesPerPixel = 4;
    backBuffer.pitch = backBuffer.width * backBuffer.bytesPerPixel;
    backBuffer.memory = malloc(backBuffer.width * backBuffer.height * backBuffer.bytesPerPixel);

    backBufferInfo.bmiHeader.biSize = sizeof(backBufferInfo.bmiHeader);
    backBufferInfo.bmiHeader.biWidth = backBuffer.width;
    backBufferInfo.bmiHeader.biHeight = -(s32)(backBuffer.height);
    backBufferInfo.bmiHeader.biPlanes = 1;
    backBufferInfo.bmiHeader.biBitCount = (WORD)(backBuffer.bytesPerPixel * 8);
    backBufferInfo.bmiHeader.biCompression = BI_RGB;

    deviceContext = GetDC(state.hwnd);
    return true;
}

void W64RendererSoftware::Render(renderList_t &renderList)
{
    u8 *base = renderList.begin();
    u8 *end = renderList.end();

    rcGroup_t *activeGroup = nullptr;
    mat4x4 identityMat = Mat4Identity();
    size_t commandCount = 0;

    for (; base != end;)
    {
        if (activeGroup)
        {
            ++commandCount;
            if (activeGroup->commandCount < commandCount)
            {
                activeGroup = nullptr;
                commandCount = 0;
            }
        }

        renderCommand_t *rc = (renderCommand_t *)base;
        size_t rc_size = sizeof(renderCommand_t);
        switch (rc->type)
        {

        case rcClearColor:
        {
            rcClearColor_t *cmd = (rcClearColor_t *)base;
            DrawBufferClear(cmd->color);
            rc_size = sizeof(rcClearColor_t);
        }
        break;

        case rcLine:
        {
            rcLine_t *cmd = (rcLine_t *)base;
            s32 x0 = (s32)(cmd->p0.x * renderList.metersToPixels);
            s32 y0 = (s32)(cmd->p0.y * renderList.metersToPixels);
            s32 x1 = (s32)(cmd->p1.x * renderList.metersToPixels);
            s32 y1 = (s32)(cmd->p1.y * renderList.metersToPixels);
            // flip y.
            y0 = renderList.windowHeight - y0;
            y1 = renderList.windowHeight - y1;
            DrawBufferLine(x0, y0, x1, y1, cmd->col0, cmd->col1);
            rc_size = sizeof(rcLine_t);
        }
        break;

        case rcTriangleOutline:
        {
            rcTriangleOutline_t *cmd = (rcTriangleOutline_t *)base;

            v4f v0 = v4f(cmd->vertices[0], 1.0f);
            v4f v1 = v4f(cmd->vertices[1], 1.0f);
            v4f v2 = v4f(cmd->vertices[2], 1.0f);

            v0 = v0 * (activeGroup ? activeGroup->model : identityMat);
            v1 = v1 * (activeGroup ? activeGroup->model : identityMat);
            v2 = v2 * (activeGroup ? activeGroup->model : identityMat);

            v0 += (activeGroup ? activeGroup->translation : v4f{0.0f});
            v1 += (activeGroup ? activeGroup->translation : v4f{0.0f});
            v2 += (activeGroup ? activeGroup->translation : v4f{0.0f});

            v0 = v0 * (activeGroup ? activeGroup->projection : identityMat);
            v1 = v1 * (activeGroup ? activeGroup->projection : identityMat);
            v2 = v2 * (activeGroup ? activeGroup->projection : identityMat);
            v0 /= v0.w;
            v1 /= v1.w;
            v2 /= v2.w;

            v0 += v4f{1.0f, 1.0f, 0.0f, 0.0f};
            v1 += v4f{1.0f, 1.0f, 0.0f, 0.0f};
            v2 += v4f{1.0f, 1.0f, 0.0f, 0.0f};

            v0 = hadamard(v0, v4f{0.5f * backBuffer.width, 0.5f * backBuffer.height, 0.0f, 0.0f});
            v1 = hadamard(v1, v4f{0.5f * backBuffer.width, 0.5f * backBuffer.height, 0.0f, 0.0f});
            v2 = hadamard(v2, v4f{0.5f * backBuffer.width, 0.5f * backBuffer.height, 0.0f, 0.0f});

            v3f t0 = v0.xyz;
            v3f t1 = v1.xyz;
            v3f t2 = v2.xyz;

            t0.y = renderList.windowHeight - t0.y;
            t1.y = renderList.windowHeight - t1.y;
            t2.y = renderList.windowHeight - t2.y;
            DrawBufferLine((s32)t0.x, (s32)t0.y, (s32)t1.x, (s32)t1.y, ageCOLOR_WHITE);
            DrawBufferLine((s32)t1.x, (s32)t1.y, (s32)t2.x, (s32)t2.y, ageCOLOR_WHITE);
            DrawBufferLine((s32)t2.x, (s32)t2.y, (s32)t0.x, (s32)t0.y, ageCOLOR_WHITE);
            rc_size = sizeof(rcTriangleOutline_t);
        }
        break;

        case rcTriangle:
        {

            rcTriangle_t *cmd = (rcTriangle_t *)base;
            v4f v0 = v4f(cmd->vertices[0], 1.0f);
            v4f v1 = v4f(cmd->vertices[1], 1.0f);
            v4f v2 = v4f(cmd->vertices[2], 1.0f);

            v0 = v0 * (activeGroup ? activeGroup->model : identityMat);
            v1 = v1 * (activeGroup ? activeGroup->model : identityMat);
            v2 = v2 * (activeGroup ? activeGroup->model : identityMat);

            v0 += (activeGroup ? activeGroup->translation : v4f{0.0f});
            v1 += (activeGroup ? activeGroup->translation : v4f{0.0f});
            v2 += (activeGroup ? activeGroup->translation : v4f{0.0f});

            v0 = v0 * (activeGroup ? activeGroup->projection : identityMat);
            v1 = v1 * (activeGroup ? activeGroup->projection : identityMat);
            v2 = v2 * (activeGroup ? activeGroup->projection : identityMat);
            v0 /= v0.w;
            v1 /= v1.w;
            v2 /= v2.w;

            v0 += v4f{1.0f, 1.0f, 0.0f, 0.0f};
            v1 += v4f{1.0f, 1.0f, 0.0f, 0.0f};
            v2 += v4f{1.0f, 1.0f, 0.0f, 0.0f};

            v0 = hadamard(v0, v4f{0.5f * backBuffer.width, 0.5f * backBuffer.height, 0.0f, 0.0f});
            v1 = hadamard(v1, v4f{0.5f * backBuffer.width, 0.5f * backBuffer.height, 0.0f, 0.0f});
            v2 = hadamard(v2, v4f{0.5f * backBuffer.width, 0.5f * backBuffer.height, 0.0f, 0.0f});

            v3f t0 = v0.xyz;
            v3f t1 = v1.xyz;
            v3f t2 = v2.xyz;

            t0.y = renderList.windowHeight - t0.y;
            t1.y = renderList.windowHeight - t1.y;
            t2.y = renderList.windowHeight - t2.y;
            if (cmd->interpolate)
                DrawBufferFillTriangle(t0, t1, t2, cmd->vert_col[0], cmd->vert_col[1], cmd->vert_col[2]);
            else
                DrawBufferFillTriangle(t0, t1, t2, cmd->vert_col[0]);
            rc_size = sizeof(rcTriangle_t);
        }
        break;
        case rcGroup:
        {
            rcGroup_t *cmd = (rcGroup_t *)base;
            activeGroup = cmd;
            rc_size = sizeof(rcGroup_t);
        }
        break;

        default:
            std::cout << "Unsupported Draw Call: " << rc->type << std::endl;
            Assert(false);
            break;
        }

        base = ((u8 *)base) + rc_size;
    }

    StretchDIBits(deviceContext, 0, 0,
                  backBuffer.width, backBuffer.height,
                  0, 0, backBuffer.width, backBuffer.height,
                  backBuffer.memory, &backBufferInfo,
                  DIB_RGB_COLORS, SRCCOPY);

    if (vsync)
    {
        Assert(DwmFlush() == S_OK);
    }
}

void W64RendererSoftware::Cleanup(const w64State_t &state)
{
}

void W64RendererSoftware::DrawBufferSetPixel(s32 x, s32 y, v4f col)
{
    if (x >= backBuffer.width || x < 0 || y >= backBuffer.height || y < 0)
        return;
    u32 *p = (u32 *)backBuffer.memory + ((y * backBuffer.width) + x);
    *p = ConvertToPackedU32(col);
}

void W64RendererSoftware::DrawBufferClear(v4f color)
{

    u32 packedColor = ConvertToPackedU32(color);

    if ((packedColor << 8) == 0)
        memset(backBuffer.memory, 0, backBuffer.width * backBuffer.height * backBuffer.bytesPerPixel);

    else
    {
        u32 *p = (u32 *)backBuffer.memory;
        for (int i = 0; i < backBuffer.width * backBuffer.height; ++i)
            *p++ = packedColor;
    }
}

void W64RendererSoftware::DrawBufferLine(s32 x0, s32 y0, s32 x1, s32 y1, v4f color0, v4f color1)
{
    // Clamp into buffer: "clipping ?"
    x0 = Contain(x0, 0, backBuffer.width - 1);
    y0 = Contain(y0, 0, backBuffer.height - 1);
    x1 = Contain(x1, 0, backBuffer.width - 1);
    y1 = Contain(y1, 0, backBuffer.height - 1);

    s32 xMin = Min(x0, x1);
    s32 xMax = Max(x0, x1);

    s32 yMin = Min(y0, y1);
    s32 yMax = Max(y0, y1);

    s32 steps = Max((yMax - yMin), (xMax - xMin));
    if (steps > 0)
    {
        f32 dx = (x1 - x0) / (f32)steps;
        f32 dy = (y1 - y0) / (f32)steps;
        f32 stepSize = 1.0f / steps;
        for (s32 step = 0; step < steps; ++step)
        {
            s32 x = Contain((s32)(x0 + (dx * step)), 0, backBuffer.width);
            s32 y = Contain((s32)(y0 + (dy * step)), 0, backBuffer.height);

            u32 *p = (u32 *)backBuffer.memory + ((y * backBuffer.width) + x);
            v4f lerpC = lerp(color0, color1, stepSize * step);
            *p = ConvertToPackedU32(lerpC);
        }
    }
}

void W64RendererSoftware::DrawBufferLine(s32 x0, s32 y0, s32 x1, s32 y1, v4f color)
{
    // Clamp into buffer: "clipping ?"
    x0 = Contain(x0, 0, backBuffer.width - 1);
    y0 = Contain(y0, 0, backBuffer.height - 1);
    x1 = Contain(x1, 0, backBuffer.width - 1);
    y1 = Contain(y1, 0, backBuffer.height - 1);

    s32 xMin = Min(x0, x1);
    s32 xMax = Max(x0, x1);

    s32 yMin = Min(y0, y1);
    s32 yMax = Max(y0, y1);

    s32 steps = Max((yMax - yMin), (xMax - xMin));
    if (steps > 0)
    {
        f32 dx = (x1 - x0) / (f32)steps;
        f32 dy = (y1 - y0) / (f32)steps;
        for (s32 step = 0; step < steps; ++step)
        {
            s32 x = Contain((s32)(x0 + (dx * step)), 0, backBuffer.width);
            s32 y = Contain((s32)(y0 + (dy * step)), 0, backBuffer.height);

            u32 *p = (u32 *)backBuffer.memory + ((y * backBuffer.width) + x);
            *p = ConvertToPackedU32(color);
        }
    }
}

// STUDY(pf): barycentric coordinates.
static f32 lineDistance2D(v2f a, v2f b, v2f p)
{
    v2f n{a.y - b.y, b.x - a.x};
    f32 d = a.x * b.y - a.y * b.x;
    return (dot(n, p) + d) / length(n);
}

static f32 bary2D(v2f a, v2f b, v2f c, v2f p)
{
    return lineDistance2D(b, c, p) / lineDistance2D(b, c, a);
}

void W64RendererSoftware::DrawBufferFillTriangle(v3f t0, v3f t1, v3f t2, v4f c0, v4f c1, v4f c2)
{
    v3f min = v3f{(f32)backBuffer.width - 1, (f32)backBuffer.height - 1, 0.0f};
    v3f max = v3f{0.0f, 0.0f, 0.0f};
    if (min.x > t0.x)
        min.x = t0.x;
    if (min.x > t1.x)
        min.x = t1.x;
    if (min.x > t2.x)
        min.x = t2.x;
    if (min.y > t0.y)
        min.y = t0.y;
    if (min.y > t1.y)
        min.y = t1.y;
    if (min.y > t2.y)
        min.y = t2.y;
    if (max.x < t0.x)
        max.x = t0.x;
    if (max.x < t1.x)
        max.x = t1.x;
    if (max.x < t2.x)
        max.x = t2.x;
    if (max.y < t0.y)
        max.y = t0.y;
    if (max.y < t1.y)
        max.y = t1.y;
    if (max.y < t2.y)
        max.y = t2.y;
    // TODO(pf): Clipping routine before this.
    for (int y = max((int)min.y, 0); y <= min((int)max.y, backBuffer.height - 1); ++y)
        for (int x = max((int)min.x, 0); x <= min((int)max.x, backBuffer.width - 1); ++x)
        {
            v2f p = {(f32)x, (f32)y};
            f32 t0_cont = bary2D(t0.xy, t1.xy, t2.xy, p);
            f32 t1_cont = bary2D(t1.xy, t2.xy, t0.xy, p);
            f32 t2_cont = bary2D(t2.xy, t0.xy, t1.xy, p);
            if (t0_cont < 0 || t1_cont < 0 || t2_cont < 0)
                continue;
            u32 *pi = (u32 *)backBuffer.memory + ((y * backBuffer.width) + x);
            v4f int_col = t0_cont * c0 + t1_cont * c1 + t2_cont * c2;
            // v4f  int_col = c0;
            // int_col = normalize(int_col);
            u32 pc = ConvertToPackedU32(int_col);
            *pi = pc;
        }
}

#define USE_BARY 1
#if USE_BARY
// barycentric fill.
void W64RendererSoftware::DrawBufferFillTriangle(v3f t0, v3f t1, v3f t2, v4f c0)
{
    v3f min = v3f{(f32)backBuffer.width - 1, (f32)backBuffer.height - 1, 0.0f};
    v3f max = v3f{0.0f, 0.0f, 0.0f};
    if (min.x > t0.x)
        min.x = t0.x;
    if (min.x > t1.x)
        min.x = t1.x;
    if (min.x > t2.x)
        min.x = t2.x;
    if (min.y > t0.y)
        min.y = t0.y;
    if (min.y > t1.y)
        min.y = t1.y;
    if (min.y > t2.y)
        min.y = t2.y;
    if (max.x < t0.x)
        max.x = t0.x;
    if (max.x < t1.x)
        max.x = t1.x;
    if (max.x < t2.x)
        max.x = t2.x;
    if (max.y < t0.y)
        max.y = t0.y;
    if (max.y < t1.y)
        max.y = t1.y;
    if (max.y < t2.y)
        max.y = t2.y;
    u32 pc = ConvertToPackedU32(c0);
    for (int y = (int)min.y; y <= (int)max.y; ++y)
        for (int x = (int)min.x; x <= (int)max.x; ++x)
        {
            v2f p = {(f32)x, (f32)y};
            if (bary2D(t0.xy, t1.xy, t2.xy, p) < 0 || bary2D(t1.xy, t2.xy, t0.xy, p) < 0 || bary2D(t2.xy, t0.xy, t1.xy, p) < 0)
                continue;
            u32 *pi = (u32 *)backBuffer.memory + ((y * backBuffer.width) + x);
            *pi = pc;
        }
}

#else

void W64RendSoftware::DrawBufferFillTriangle(w64Rendering_t *rend, v3f t0, v3f t1, v3f t2,
                                             v4f c0)
{

    if (t0.y > t1.y)
    {
        std::swap(t0, t1);
        // std::cout << "swapping t0, t1: " << t0.y - t1.y << std::endl;
    }
    if (t0.y > t2.y)
    {
        std::swap(t0, t2);
        // std::cout << "swapping t1, t2: " << t0.y - t2.y << std::endl;
    }
    if (t1.y > t2.y)
    {
        std::swap(t1, t2);
        // std::cout << "swapping t2, t0: " << t1.y - t2.y << std::endl;
    }

    Assert(t2.y >= t1.y && t1.y >= t0.y);
    if (t1.y == t0.y)
        return;

    f32 invTotH = 1 / (t2.y - t0.y);
    f32 invSegH = 1 / (t1.y - t0.y + 1);
    // NOTE(pf): First part of triangle, t0->t1
    for (int y = (int)t0.y; y <= (int)t1.y; ++y)
    {
        f32 alpha = (f32)(y - t0.y) * invTotH;
        f32 beta = (f32)(y - t0.y) * invSegH;

        v3f a = t0 + (t2 - t0) * alpha;
        v3f b = t0 + (t1 - t0) * beta;
        if (a.x > b.x)
            std::swap(a, b);

        for (int x = (int)a.x; x < (int)b.x; ++x)
            DrawBufferSetPixel(x, y, c0);

        // Sleep(20);
        // RenderingFlip(rend);
    }
    // NOTE(pf): 2nd part of tri, t1->t2
    invSegH = 1 / (t2.y - t1.y + 1);
    for (int y = (int)t1.y; y <= (int)t2.y; ++y)
    {
        f32 alpha = (f32)(y - t0.y) * invTotH;
        f32 beta = (f32)(y - t1.y) * invSegH;

        v3f a = t0 + (t2 - t0) * alpha;
        v3f b = t1 + (t2 - t1) * beta;

        if (a.x > b.x)
            std::swap(a, b);

        for (int x = (int)a.x; x < (int)b.x; ++x)
            DrawBufferSetPixel(x, y, c0);
        // Sleep(20);
        // RenderingFlip(rend);
    }
}

#endif