#if !defined(W64_RENDERER_H)
/* ========================================================================
   Creator: Grimleik $
   ========================================================================*/
#define W64_RENDERER_H

#include "w64_main.h"
#include "age.h"

class IW64Renderer
{
  public:
	virtual ~IW64Renderer(){};
	virtual bool Init(w64State_t &state) = 0;
	virtual void Render(renderList_t &renderList) = 0;
	virtual void Cleanup(const w64State_t &state) = 0;

	static std::unique_ptr<IW64Renderer> Create(RENDER_MODE, const w64State_t &state);
	static std::unique_ptr<IW64Renderer> Instance;
};

#endif
