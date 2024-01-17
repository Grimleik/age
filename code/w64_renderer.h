#if !defined(W64_RENDERER_H)
/* ========================================================================
   Creator: Grimleik $
   ========================================================================*/
#define W64_RENDERER_H

#include "w64_main.h"
#include "age.h"

class IW64Renderer {
  public:
    virtual ~IW64Renderer(){};
    virtual void Init(w64State_t &state) = 0;
    virtual void Render(renderList_t &renderList) = 0;
    virtual void Cleanup() = 0;

    static std::unique_ptr<IW64Renderer> Create(RENDER_MODE);
    static std::unique_ptr<IW64Renderer> Instance;
};

#endif
