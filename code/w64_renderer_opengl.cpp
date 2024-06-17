/* ========================================================================
   Creator: Grimleik $
   ========================================================================*/
#include <glad/glad.h>
#include "w64_renderer_opengl.h"

const char *vertexShaderSource = "#version 330 core \n"
								 "layout (location = 0) in vec3 aPos; \n"
								 "void main() \n"
								 "{ \n"
								 "	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
								 "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
								   "out vec4 FragColor;\n"
								   "void main()\n"
								   "{\n"
								   "	FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
								   "}\0";

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

	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
				  << infoLog << std::endl;
	}

	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
				  << infoLog << std::endl;
	}

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glGetShaderiv(shaderProgram, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n"
				  << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
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
		case rcClearColor:
		{
			rcClearColor_t *cmd = (rcClearColor_t *)base;
			glClearColor(cmd->color.r, cmd->color.g, cmd->color.b, cmd->color.a);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			rc_size = sizeof(rcClearColor_t);
		}
		break;
		case rcGroup:
		{
			rcGroup_t *cmd = (rcGroup_t *)base;
			activeGroup = cmd;
			rc_size = sizeof(rcGroup_t);
		}
		break;
		case rcTriangleOutline:
		{
			rc_size = sizeof(rcTriangleOutline_t);
			// mat4x4 model = Mat4Identity();
			// rcTriangleOutline_t *cmd = (rcTriangleOutline_t *)base;
			// unsigned int VBO, VAO;
			// glGenVertexArrays(1, &VAO);
			// glGenBuffers(1, &VBO);
			// glBindVertexArray(VAO);
			// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
			// glEnableVertexAttribArray(0);
			// glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		break;
		case rcAsset:
		{
			rcAsset_t *cmd = (rcAsset_t *)base;
			if (!cmd->asset->isLoaded)
			{
				unsigned int VBO, VAO, EBO;
				glGenVertexArrays(1, &VAO);
				glGenBuffers(1, &VBO);
				glGenBuffers(1, &EBO);
				glBindVertexArray(VAO);
				glBindBuffer(GL_ARRAY_BUFFER, VBO);
				glBufferData(GL_ARRAY_BUFFER,
							 cmd->asset->mesh.vertices.size() * sizeof(sizeof(cmd->asset->mesh.vertices[0])),
							 &cmd->asset->mesh.vertices[0], GL_STATIC_DRAW);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER,
							 sizeof(cmd->asset->mesh.indices[0]) * cmd->asset->mesh.indices.size(),
							 &cmd->asset->mesh.indices[0], GL_STATIC_DRAW);

				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
				glEnableVertexAttribArray(0);

				assetIDToVAO[cmd->asset->ID] = VAO;

				cmd->asset->isLoaded = true;
				// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}

			glUseProgram(shaderProgram);
			glBindVertexArray(assetIDToVAO[cmd->asset->ID]);
			glDrawElements(GL_TRIANGLES,
						   (GLsizei)cmd->asset->mesh.indices.size(),
						   GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			rc_size = sizeof(rcAsset_t);
		}
		break;

		default:
			// std::cout << "Unsupported Draw Call: " << rc->type << std::endl;
			// Assert(false);
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
