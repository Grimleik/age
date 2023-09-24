#if !defined(W64_SOFTWARE_RENDERING_H)
/* ========================================================================
   Creator: Grimleik $
   ========================================================================*/
#define W64_SOFTWARE_RENDERING_H

#include "w64_main.h"
namespace sr {

struct w64Drawbuffer_t {
    void *memory;
    s32   width;
    s32   height;
    u32   pitch;
    u32   bytesPerPixel;
};

struct w64Rendering_t {
    HDC             deviceContext;
    w64Drawbuffer_t backBuffer;
    BITMAPINFO      backBufferInfo;
    b32             vsync;
};

w64Rendering_t RenderingInit(w64State_t *handle);
void           RenderingFrame(renderList_t *rl, w64Rendering_t *rend);
void           RenderingFlip(w64Rendering_t *rs);

}; // namespace sr
#endif
