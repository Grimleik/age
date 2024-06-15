/* ========================================================================
   Creator: Grimleik $
   ========================================================================*/
#include <glad/glad.h>
#include "w64_renderer_opengl.h"

W64RendOpenGL::W64RendOpenGL()
{
}

W64RendOpenGL::~W64RendOpenGL()
{
}

void *GetExtensionFuncAddress(const char *name)
{
	void *p = (void *)wglGetProcAddress(name);
	if (p == 0 || (p == (void *)0x1) ||
		(p == (void *)0x2) || (p == (void *)0x3) || (p == (void *)-1))
	{
		HMODULE module = LoadLibraryA("opengl32.dll");
		p = (void *)GetProcAddress(module, name);
	}
	return p;
}

bool W64RendOpenGL::Init(w64State_t &state)
{
	deviceContext = GetDC(state.hwnd);

	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(pfd));

	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 8;
	pfd.iLayerType = PFD_MAIN_PLANE;

	int pixelFormat = ChoosePixelFormat(deviceContext, &pfd);
	if (pixelFormat == 0)
	{
		return false;
	}

	if (SetPixelFormat(deviceContext, pixelFormat, &pfd) == FALSE)
	{
		return false;
	}

	renderContext = wglCreateContext(deviceContext);

	if (renderContext == NULL)
	{
		return false;
	}

	if (wglMakeCurrent(deviceContext, renderContext) != TRUE)
	{
		return false;
	}

	if (!gladLoadGLLoader((GLADloadproc)GetExtensionFuncAddress))
	{
		// TODO: Logging.
		return false;
	}

	glViewport(0, 0, state.windowWidth, state.windowHeight);

	return true;
}

void W64RendOpenGL::Render(renderList_t &renderList)
{
	u8 *base = renderList.begin();
	u8 *end = renderList.end();

	rcGroup_t *activeGroup = nullptr;
	mat4x4 identity = Mat4Identity();
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
		case rcClearColor: {
			rcClearColor_t *cmd = (rcClearColor_t *)base;
			glClearColor(cmd->color.r, cmd->color.g, cmd->color.b, cmd->color.a);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			rc_size = sizeof(rcClearColor_t);
		}
		break;
		case rcGroup: {
			rcGroup_t *cmd = (rcGroup_t *)base;
			activeGroup = cmd;
			rc_size = sizeof(rcGroup_t);
		}
		break;
		case rcTriangleOutline: {
			rc_size = sizeof(rcTriangleOutline_t);
		}break;

		default:
            //std::cout << "Unsupported Draw Call: " << rc->type << std::endl;
            //Assert(false);
            break;
		}

		base = ((u8 *)base + rc_size);
	}
	SwapBuffers(deviceContext);
}

void W64RendOpenGL::Cleanup(const w64State_t &state)
{
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(renderContext);
	ReleaseDC(state.hwnd, deviceContext);
}
