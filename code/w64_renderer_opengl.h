#if !defined(W64_OPENGL_H)
/* ========================================================================
   Creator: Grimleik $
   ========================================================================*/
#define W64_OPENGL_H
#include "w64_renderer.h"

#pragma comment(lib, "opengl32.lib")

class W64RendOpenGL : public IW64Renderer
{
public:
    W64RendOpenGL();
    ~W64RendOpenGL();

    bool Init(w64State_t &state) override;
    void Render(renderList_t &renderList) override;
    void Cleanup(const w64State_t &state) override;

private:
    HDC deviceContext;
    HGLRC renderContext;
    std::map<int, int> assetIDToVAO;

  	unsigned int shaderProgram;
};

#endif