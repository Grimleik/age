#if !defined(W64_RENDERER_SOFTWARE_H)
/* ========================================================================
   Creator: Grimleik $
   ========================================================================*/
#define W64_RENDERER_SOFTWARE_H
#include "w64_renderer.h"

#pragma comment(lib, "winmm.lib")

class W64RendSoftware : public IW64Renderer {

  public:
    W64RendSoftware();
    ~W64RendSoftware();
    bool Init(w64State_t &state) override;
    void Render(renderList_t &renderList) override;
    void Cleanup(const w64State_t& state) override;

    struct back_buffer_t {
        void *memory;
        s32   width;
        s32   height;
        u32   pitch;
        u32   bytesPerPixel;
    };

  private:
    HDC           deviceContext;
    BITMAPINFO    backBufferInfo;
    b32           vsync;
    back_buffer_t backBuffer;

    void DrawBufferSetPixel(s32 x, s32 y, v4f col);
    void DrawBufferClear(v4f color);
    void DrawBufferLine(s32 x0, s32 y0, s32 x1, s32 y1, v4f color0, v4f color1);
    void DrawBufferLine(s32 x0, s32 y0, s32 x1, s32 y1, v4f color);
    void DrawBufferFillTriangle(v3f t0, v3f t1, v3f t2, v4f c0, v4f c1, v4f c2);
    void DrawBufferFillTriangle(v3f t0, v3f t1, v3f t2, v4f c0);
};

#endif
