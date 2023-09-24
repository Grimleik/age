/* ========================================================================
   Creator: Grimleik $
   ========================================================================*/
#include "age.h"
#include "w64_software_rendering.h"
#include <iostream>

#include <dwmapi.h>
#pragma comment(lib, "Dwmapi.lib")
namespace sr {

static void DrawBufferSetPixel(w64Drawbuffer_t *buffer, s32 x, s32 y, v4f col) {
    if (x >= buffer->width || x < 0 || y >= buffer->height || y < 0)
        return;
    u32 *p = (u32 *)buffer->memory + ((y * buffer->width) + x);
    *p = ConvertToPackedU32(col);
}

static void DrawBufferClear(w64Drawbuffer_t *buffer, v4f color) {

    u32 packedColor = ConvertToPackedU32(color);

    if ((packedColor << 8) == 0)
        memset(buffer->memory, 0, buffer->width * buffer->height * buffer->bytesPerPixel);

    else {
        u32 *p = (u32 *)buffer->memory;
        for (int i = 0; i < buffer->width * buffer->height; ++i)
            *p++ = packedColor;
    }
}

static void DrawBufferLine(w64Drawbuffer_t *buffer, s32 x0, s32 y0, s32 x1, s32 y1,
                           v4f color0, v4f color1) {
    // Clamp into buffer: "clipping ?"
    x0 = Contain(x0, 0, buffer->width - 1);
    y0 = Contain(y0, 0, buffer->height - 1);
    x1 = Contain(x1, 0, buffer->width - 1);
    y1 = Contain(y1, 0, buffer->height - 1);

    s32 xMin = Min(x0, x1);
    s32 xMax = Max(x0, x1);

    s32 yMin = Min(y0, y1);
    s32 yMax = Max(y0, y1);

    s32 steps = Max((yMax - yMin), (xMax - xMin));
    if (steps > 0) {
        f32 dx = (x1 - x0) / (f32)steps;
        f32 dy = (y1 - y0) / (f32)steps;
        f32 stepSize = 1.0f / steps;
        for (s32 step = 0; step < steps; ++step) {
            s32 x = Contain((s32)(x0 + (dx * step)), 0, buffer->width);
            s32 y = Contain((s32)(y0 + (dy * step)), 0, buffer->height);

            u32 *p = (u32 *)buffer->memory + ((y * buffer->width) + x);
            v4f  lerpC = lerp(color0, color1, stepSize * step);
            *p = ConvertToPackedU32(lerpC);
        }
    }
}

static void DrawBufferLine(w64Drawbuffer_t *buffer, s32 x0, s32 y0, s32 x1, s32 y1,
                           v4f color) {
    // Clamp into buffer: "clipping ?"
    x0 = Contain(x0, 0, buffer->width - 1);
    y0 = Contain(y0, 0, buffer->height - 1);
    x1 = Contain(x1, 0, buffer->width - 1);
    y1 = Contain(y1, 0, buffer->height - 1);

    s32 xMin = Min(x0, x1);
    s32 xMax = Max(x0, x1);

    s32 yMin = Min(y0, y1);
    s32 yMax = Max(y0, y1);

    s32 steps = Max((yMax - yMin), (xMax - xMin));
    if (steps > 0) {
        f32 dx = (x1 - x0) / (f32)steps;
        f32 dy = (y1 - y0) / (f32)steps;
        for (s32 step = 0; step < steps; ++step) {
            s32 x = Contain((s32)(x0 + (dx * step)), 0, buffer->width);
            s32 y = Contain((s32)(y0 + (dy * step)), 0, buffer->height);

            u32 *p = (u32 *)buffer->memory + ((y * buffer->width) + x);
            *p = ConvertToPackedU32(color);
        }
    }
}

static f32 lineDistance2D(v2f a, v2f b, v2f p) {
    v2f n{a.y - b.y, b.x - a.x};
    f32 d = a.x * b.y - a.y * b.x;
    return (dot(n, p) + d) / length(n);
}

static f32 bary2D(v2f a, v2f b, v2f c, v2f p) { 
    return lineDistance2D(b, c, p) / lineDistance2D(b, c, a);
}

// barycentric fill.
static void DrawBufferFillTriangle(w64Rendering_t *rend, v3f t0, v3f t1, v3f t2,
                                   v4f c0) {
    v3f min = v3f{(f32)rend->backBuffer.width - 1, (f32)rend->backBuffer.height - 1, 0.0f};
    v3f max = v3f{0.0f, 0.0f, 0.0f};
    if (min.x > t0.x) min.x = t0.x;
    if (min.x > t1.x) min.x = t1.x;
    if (min.x > t2.x) min.x = t2.x;
    if (min.y > t0.y) min.y = t0.y;
    if (min.y > t1.y) min.y = t1.y;
    if (min.y > t2.y) min.y = t2.y;
    if (max.x < t0.x) max.x = t0.x;
    if (max.x < t1.x) max.x = t1.x;
    if (max.x < t2.x) max.x = t2.x;
    if (max.y < t0.y) max.y = t0.y;
    if (max.y < t1.y) max.y = t1.y;
    if (max.y < t2.y) max.y = t2.y;
    DrawBufferLine(&rend->backBuffer, (s32)t0.x, (s32)t0.y, (s32)t1.x, (s32)t1.y, ageCOLOR_WHITE);
    DrawBufferLine(&rend->backBuffer, (s32)t1.x, (s32)t1.y, (s32)t2.x, (s32)t2.y, ageCOLOR_WHITE);
    DrawBufferLine(&rend->backBuffer, (s32)t2.x, (s32)t2.y, (s32)t0.x, (s32)t0.y, ageCOLOR_WHITE);
    u32 pc = ConvertToPackedU32(c0);
    for (int y = (int)min.y; y < (int)max.y; ++y)
        for (int x = (int)min.x; x < (int)max.x; ++x) {
            // v3f bary = v3f{(f32)x, (f32)y, 0};
            // v3f bary = BarycentricCoordinate(t0, t1, t2, v3f{(f32)x, (f32)y, 0});
            // if (bary.x < 0 || bary.y < 0 || bary.z < 0) continue;
            v2f p = {(f32)x, (f32)y};
            if(bary2D(t0.xy, t1.xy, t2.xy, p) < 0 || bary2D(t1.xy, t2.xy, t0.xy, p) < 0 || bary2D(t2.xy, t0.xy, t1.xy, p) < 0) continue;
            u32 *pi = (u32 *)rend->backBuffer.memory + ((y * rend->backBuffer.width) + x);
            *pi = pc;
        }
}

#if 0 // Piece-wise fill triangle.
static void DrawBufferFillTriangle(w64Rendering_t *rend, v3f t0, v3f t1, v3f t2,
                                   v4f c0) {

    if (t0.y > t1.y) {
        std::swap(t0, t1);
        // std::cout << "swapping t0, t1: " << t0.y - t1.y << std::endl;
    }
    if (t0.y > t2.y) {
        std::swap(t0, t2);
        // std::cout << "swapping t1, t2: " << t0.y - t2.y << std::endl;
    }
    if (t1.y > t2.y) {
        std::swap(t1, t2);
        // std::cout << "swapping t2, t0: " << t1.y - t2.y << std::endl;
    }

    Assert(t2.y >= t1.y && t1.y >= t0.y);
    if (t1.y == t0.y)
        return;

    // OUTLINE
    DrawBufferLine(&rend->backBuffer, (s32)t0.x, (s32)t0.y, (s32)t1.x, (s32)t1.y, ageCOLOR_WHITE);
    DrawBufferLine(&rend->backBuffer, (s32)t1.x, (s32)t1.y, (s32)t2.x, (s32)t2.y, ageCOLOR_WHITE);
    DrawBufferLine(&rend->backBuffer, (s32)t2.x, (s32)t2.y, (s32)t0.x, (s32)t0.y, ageCOLOR_WHITE);

    f32 invTotH = 1 / (t2.y - t0.y);
    f32 invSegH = 1 / (t1.y - t0.y + 1);
    // NOTE(pf): First part of triangle, t0->t1
    for (int y = (int)t0.y; y <= (int)t1.y; ++y) {
        f32 alpha = (f32)(y - t0.y) * invTotH;
        f32 beta = (f32)(y - t0.y) * invSegH;

        v3f a = t0 + (t2 - t0) * alpha;
        v3f b = t0 + (t1 - t0) * beta;
        if (a.x > b.x)
            std::swap(a, b);

        for (int x = (int)a.x; x < (int)b.x; ++x)
            DrawBufferSetPixel(&rend->backBuffer, x, y, c0);

        Sleep(20);
        RenderingFlip(rend);
    }
    // NOTE(pf): 2nd part of tri, t1->t2
    invSegH = 1 / (t2.y - t1.y + 1);
    for (int y = (int)t1.y; y <= (int)t2.y; ++y) {
        f32 alpha = (f32)(y - t0.y) * invTotH;
        f32 beta = (f32)(y - t1.y) * invSegH;

        v3f a = t0 + (t2 - t0) * alpha;
        v3f b = t1 + (t2 - t1) * beta;

        if (a.x > b.x)
            std::swap(a, b);

        for (int x = (int)a.x; x < (int)b.x; ++x)
            DrawBufferSetPixel(&rend->backBuffer, x, y, c0);
        Sleep(20);
        RenderingFlip(rend);
    }
}
#endif
w64Rendering_t RenderingInit(w64State_t *handle) {

    w64Rendering_t result = {0};
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

void RenderingFrame(renderList_t *rl, w64Rendering_t *rend) {

    u8 *base = (u8 *)rl->renderMemory;
    u8 *end = (u8 *)rl->renderMemory + rl->renderMemoryCurrSz;
    for (; base != end;) {

        renderCommand_t *rc = (renderCommand_t *)base;
        size_t           rc_size = sizeof(renderCommand_t);
        switch (rc->type) {

        case rcClearColor: {
            rcClearColor_t *cmd = (rcClearColor_t *)base;
            DrawBufferClear(&rend->backBuffer, cmd->color);
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
            DrawBufferLine(&rend->backBuffer, x0, y0, x1, y1, cmd->col0, cmd->col1);
            rc_size = sizeof(rcLine_t);
        } break;

        case rcTriangleOutline: {
            rcTriangleOutline_t *cmd = (rcTriangleOutline_t *)base;

            v3f t0 = cmd->vertices[0] * rl->metersToPixels;
            v3f t1 = cmd->vertices[1] * rl->metersToPixels;
            v3f t2 = cmd->vertices[2] * rl->metersToPixels;

            t0.y = rl->windowHeight - t0.y;
            t1.y = rl->windowHeight - t1.y;
            t2.y = rl->windowHeight - t2.y;
            DrawBufferFillTriangle(rend, t0, t1, t2, cmd->vert_col[0]);
            rc_size = sizeof(rcTriangleOutline_t);

        } break;

        default:
            std::cout << "Unsupported Draw Call: " << rc->type << std::endl;
            Assert(false);
            break;
        }

        base = ((u8 *)base) + rc_size;
    }
}

void RenderingFlip(w64Rendering_t *rs) {
    StretchDIBits(rs->deviceContext, 0, 0,
                  rs->backBuffer.width, rs->backBuffer.height,
                  0, 0, rs->backBuffer.width, rs->backBuffer.height,
                  rs->backBuffer.memory, &rs->backBufferInfo,
                  DIB_RGB_COLORS, SRCCOPY);

    if (rs->vsync) {
        Assert(DwmFlush() == S_OK);
    }
}
}; // namespace sr