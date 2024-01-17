#if !defined(W64_OPENGL_H)
/* ========================================================================
   Creator: Grimleik $
   ========================================================================*/
#define W64_OPENGL_H
#include "w64_renderer.h"
class W64RendOpenGL : public IW64Renderer {
  public:
    W64RendOpenGL();
    ~W64RendOpenGL();

    void Init(w64State_t &state) override;
    void Render(renderList_t &renderList) override;
    void Cleanup() override;
};

#endif