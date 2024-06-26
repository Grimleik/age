/* ========================================================================
   Creator: Grimleik $
   ========================================================================*/

#include "w64_renderer.h"

std::unique_ptr<IW64Renderer> IW64Renderer::Instance = nullptr;

std::unique_ptr<IW64Renderer> IW64Renderer::Create(RENDER_MODE mode, const w64State_t &state) {
    if(Instance) {
        Instance->Cleanup(state);
        Instance = nullptr;
    }
    switch (mode) {
    case Software: {
        Instance = std::make_unique<W64RendererSoftware>();
    } break;
    case OpenGL: {
        Instance = std::make_unique<W64RendOpenGL>();
    } break;
    default:
        Assert(true && "Unsupported Rendering Mode.");
        break;
    }
    return std::move(Instance);
}