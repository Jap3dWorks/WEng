// https://gist.github.com/mortennobel/0e9e90c9bbc61cc99d5c3e9c038d8115
// https://github.com/mortennobel/emscripten-cubes/blob/master/main.cpp

#include <iostream>
#include <string>
#include <vector>
#include <functional>

#include <GL/glew.h>
#include <GL/glut.h>
#include <SDL2/SDL.h>
#include <stb/stb_image.h>

#include <chrono>
#include <memory>

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"

GLuint program;
GLuint vbo_triangle;

struct FCamera
{
	glm::vec3 Position = glm::vec3(0.f, 0.f, 3.f);
	glm::vec3 CameraFront = glm::vec3(0.f, 0.f, -1.f);
	glm::vec3 CameraUp = glm::vec3(0.f, 1.f, 0.f);

	float Yaw = -90.f;
	float Pitch = 0.f;
	float LastX = 400.f;
	float LastY = 300.f;
	float FOV = 45.f;

	float CameraSpeed = 0.05f;
};

struct FVertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

struct FMesh
{
	std::vector<FVertex> Vertices;
	std::vector<uint32_t> Indices;
	// std::vector<GLuint> Textures;
};

namespace KeyCallbacks
{
	void MouseCameraCallback(
		const float& InX, 
		const float& InY, 
		FCamera& InCamera
	)
	{
		float xoffset = InX - InCamera.LastX;
		float yoffset = InCamera.LastY - InY;
		InCamera.LastX = InX;
		InCamera.LastY = InY;

		float sensitivity = 0.1f;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		InCamera.Yaw += xoffset;
		InCamera.Pitch += yoffset;

		if(InCamera.Pitch > 89.f)
			InCamera.Pitch = 89.f;
		if(InCamera.Pitch < -89.f)
			InCamera.Pitch = -89.f;

		glm::vec3 front;
		front.x = cos(glm::radians(InCamera.Yaw)) * cos(glm::radians(InCamera.Pitch));
		front.y = sin(glm::radians(InCamera.Pitch));
		front.z = sin(glm::radians(InCamera.Yaw)) * cos(glm::radians(InCamera.Pitch));
		InCamera.CameraFront = glm::normalize(front);
	}

	void KeyboardCameraCallback(
		const int& InKey, 
		const int& InScancode, 
		FCamera& InCamera
	)
	{
		float cameraSpeed = 0.05f;
		if (InKey == SDLK_w)
		{
			InCamera.Position += InCamera.CameraSpeed * InCamera.CameraFront;
		}
		if (InKey == SDLK_s)
		{
			InCamera.Position -= InCamera.CameraSpeed * InCamera.CameraFront;
		}
		if (InKey == SDLK_a)
		{
			InCamera.Position -= glm::normalize(
									glm::cross(
										InCamera.CameraFront, 
										InCamera.CameraUp
									)
								) * InCamera.CameraSpeed;
		}
		if (InKey == SDLK_d)
		{
			InCamera.Position += glm::normalize(
									glm::cross(
										InCamera.CameraFront, 
										InCamera.CameraUp
									)
								) * InCamera.CameraSpeed;
		}
	}
}

namespace Shapes
{
	FMesh CreateCube()
	{
		FMesh Mesh;

		Mesh.Vertices = {
			// Front
			{{-0.5f, -0.5f,  0.5f}, {0.f, 0.f, 1.f}, {0.f, 0.f}},
			{{ 0.5f, -0.5f,  0.5f}, {0.f, 0.f, 1.f}, {1.f, 0.f}},
			{{ 0.5f,  0.5f,  0.5f}, {0.f, 0.f, 1.f}, {1.f, 1.f}},
			{{-0.5f,  0.5f,  0.5f}, {0.f, 0.f, 1.f}, {0.f, 1.f}},
			// Back
			{{-0.5f, -0.5f, -0.5f}, {0.f, 0.f, -1.f}, {1.f, 0.f}},
			{{ 0.5f, -0.5f, -0.5f}, {0.f, 0.f, -1.f}, {0.f, 0.f}},
			{{ 0.5f,  0.5f, -0.5f}, {0.f, 0.f, -1.f}, {0.f, 1.f}},
			{{-0.5f,  0.5f, -0.5f}, {0.f, 0.f, -1.f}, {1.f, 1.f}},
			// Left
			{{-0.5f,  0.5f,  0.5f}, {-1.f, 0.f, 0.f}, {0.f, 1.f}},
			{{-0.5f,  0.5f, -0.5f}, {-1.f, 0.f, 0.f}, {1.f, 1.f}},
			{{-0.5f, -0.5f, -0.5f}, {-1.f, 0.f, 0.f}, {1.f, 0.f}},
			{{-0.5f, -0.5f,  0.5f}, {-1.f, 0.f, 0.f}, {0.f, 0.f}},
			// Right
			{{ 0.5f,  0.5f,  0.5f}, {1.f, 0.f, 0.f}, {1.f, 1.f}},
			{{ 0.5f,  0.5f, -0.5f}, {1.f, 0.f, 0.f}, {0.f, 1.f}},
			{{ 0.5f, -0.5f, -0.5f}, {1.f, 0.f, 0.f}, {0.f, 0.f}},
			{{ 0.5f, -0.5f,  0.5f}, {1.f, 0.f, 0.f}, {1.f, 0.f}},
			// Upper
			{{-0.5f,  0.5f,  0.5f}, {0.f, 1.f, 0.f}, {0.f, 1.f}},
			{{-0.5f,  0.5f, -0.5f}, {0.f, 1.f, 0.f}, {0.f, 0.f}},
			{{ 0.5f,  0.5f, -0.5f}, {0.f, 1.f, 0.f}, {1.f, 0.f}},
			{{ 0.5f,  0.5f,  0.5f}, {0.f, 1.f, 0.f}, {1.f, 1.f}},
			// Bottom
			{{-0.5f, -0.5f,  0.5f}, {0.f, -1.f, 0.f}, {1.f, 1.f}},
			{{-0.5f, -0.5f, -0.5f}, {0.f, -1.f, 0.f}, {1.f, 0.f}},
			{{ 0.5f, -0.5f, -0.5f}, {0.f, -1.f, 0.f}, {0.f, 0.f}},
			{{ 0.5f, -0.5f,  0.5f}, {0.f, -1.f, 0.f}, {0.f, 1.f}}

		};

		Mesh.Indices = {
			0, 1, 2, 
			2, 3, 0,
			4, 5, 6, 
			6, 7, 4,
			8, 9, 10, 
			10, 11, 8,
			12, 13, 14, 
			14, 15, 12,
			16, 17, 18, 
			18, 19, 16,
			20, 21, 22, 
			22, 23, 20
		};

		return Mesh;
	}
}

class FRenderData
{
public:
	static inline uint32_t SRC_WIDTH{800};
	static inline uint32_t SRC_HEIGHT{600};

private:
	float DeltaTime = 0.f;
	float LastFrame =  0.f;

public:
	[[nodiscard]] float GetDeltaTime() const { return DeltaTime; }
	[[nodiscard]] float GetLastFrame() const { return LastFrame; }
};

class FInputActions
{
public:

	std::vector<std::function<void(const float&, const float&)>> MouseCallbacks;

	std::vector<std::function<void(const int&, const int&)>> KeyCallbacks;

	void RunMouseActions(
		const SDL_Event& InEvent
	) const
	{
		for(auto& Callback : MouseCallbacks)
		{
			Callback(InEvent.motion.x, InEvent.motion.y);
		}
	}

	void RunKeyActions(
		const SDL_Event& InEvent
	) const
	{
		for(auto& Callback : KeyCallbacks)
		{
			Callback(InEvent.key.keysym.sym, InEvent.key.keysym.scancode);
		}
	}
};

struct FSdlContext
{
	SDL_Window* Window;
	SDL_GLContext Context;
};

bool InitResources(void)
{
	GLint compile_ok = GL_FALSE, link_ok = GL_FALSE;
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	const GLchar* vs_source = R"(
		#version 460
		in vec2 coord2d;
		in vec3 VertexColor;
		out vec3 OutColor;

		void main()
		{
			// ... 
			gl_Position = vec4(coord2d, 0.f, 1.f);
			OutColor = VertexColor;
		}
	)";

	glShaderSource(vs, 1, &vs_source, NULL);
	glCompileShader(vs);
	glGetShaderiv(vs, GL_COMPILE_STATUS, &compile_ok);
	if (!compile_ok)
	{
		std::cerr << "Error in vertex shader" << std::endl;
		return false;
	}

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	const char* fs_source = R"(
		#version 460
		in vec3 OutColor;
		in vec4 gl_FragCoord;
		out vec4 DiffuseColor;

		void main()
		{
			gl_FragCoord;
			DiffuseColor = vec4(gl_FragCoord.y * 0.5, gl_FragCoord.y * 0.5, gl_FragCoord.y * 0.5, 1.0);
		}
	)";

	glShaderSource(fs, 1, &fs_source, NULL);
	glCompileShader(fs);
	glGetShaderiv(fs, GL_COMPILE_STATUS, &compile_ok);
	if (!compile_ok)
	{
		std::cerr << "Error in fragment shader" << std::endl;
		return false;
	}

	program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
	if (!link_ok)
	{
		std::cerr << "Error in glLinkProgram" << std::endl;
		return false;
	}

	GLint attribute_coord2d;
	const char* attribute_name = "coord2d";
	attribute_coord2d = glGetAttribLocation(program, attribute_name);

	GLint attribute_color;
	const char* attribute_color_name = "VertexColor";
	attribute_color = glGetAttribLocation(program, attribute_color_name);

	if (attribute_coord2d == -1)
	{
		std::cerr << "Could not bind attribute " << attribute_name << std::endl;
		return false;
	}

	GLfloat triangle_vertices[] = {
		0.0,  0.8, 1.f, 0.f, 0.f, 
	   -0.8, -0.8, 0.f, 1.f, 0.f,
		0.8, -0.8, 1.f, 0.f, 1.f
	};

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(
		GL_ARRAY_BUFFER, 
		sizeof(triangle_vertices),
		triangle_vertices,
		GL_STATIC_DRAW
	);

	glUseProgram(program);
	glEnableVertexAttribArray(
		attribute_coord2d
	);
	glVertexAttribPointer(
		attribute_coord2d,
		2,
		GL_FLOAT,
		GL_FALSE,
		5 * sizeof(GLfloat),
		0
	);
	glEnableVertexAttribArray(
		attribute_color
	);
	glVertexAttribPointer(
		attribute_color,
		3,
		GL_FLOAT,
		GL_FALSE,
		5 * sizeof(GLfloat),
		(GLvoid*)(2 * sizeof(GLfloat))
	);

	return true;
}

void Render(SDL_Window* window)
{
	// Filled in later
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	SDL_GL_SwapWindow(window);
}

void FreeResources()
{
	glDeleteProgram(program);
}

void MainLoop(
	SDL_Window* window, 
	const FInputActions& InInputActions
)
{
	while (true)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_QUIT:
					return;

				case SDL_KEYDOWN:
					InInputActions.RunKeyActions(event);
					// switch(event.key.keysym.sym)
					// {
					// 	case SDLK_LEFT: break;
					// 	case SDLK_RIGHT: break;
					// 	case SDLK_UP: break;
					// 	case SDLK_DOWN: break;
					// }
					break;

				case SDL_MOUSEMOTION:
					InInputActions.RunMouseActions(event);
					// event.motion.x; event.motion.y;
					break;
			}
		}
		Render(window);
	}
}

FSdlContext InitSDL()
{
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	SDL_Window* window = SDL_CreateWindow(
		"OpenGL SDL2 Window",
		SDL_WINDOWPOS_CENTERED, 
		SDL_WINDOWPOS_CENTERED, 
		800, 600, 
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL
	);

	if(!window)
	{
		std::cerr << "Error: " << SDL_GetError() << std::endl;
		return {nullptr, nullptr};
	}

	SDL_GLContext context = SDL_GL_CreateContext(window);

	// SDL_CALLBACK
	GLenum glew_status = glewInit();

	if (glew_status != GLEW_OK)
	{
		std::cerr << "Error: " << glewGetErrorString(glew_status) << std::endl;
		return {nullptr, nullptr};
	}

	return {window, context};
}

int main(int argc, char* argv[])
{
	FSdlContext SdlContext = InitSDL();

	if (!InitResources())
		return EXIT_FAILURE;
	
	FCamera Camera;
	FInputActions InputActions;

	InputActions.MouseCallbacks.push_back(
		[&Camera](const float& InX, const float& InY)
		{
			KeyCallbacks::MouseCameraCallback(
				InX, InY, Camera
			);
		}
	);

	InputActions.KeyCallbacks.push_back(
		[&Camera](const int& InKey, const int& InScancode)
		{
			KeyCallbacks::KeyboardCameraCallback(
				InKey, InScancode, Camera
			);
		}
	);

	MainLoop(SdlContext.Window, InputActions);

	FreeResources();

	SDL_GL_DeleteContext(SdlContext.Context);
	SDL_DestroyWindow(SdlContext.Window);
	SDL_Quit();

	return EXIT_SUCCESS;
}
