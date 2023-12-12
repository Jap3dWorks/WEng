// https://gist.github.com/mortennobel/0e9e90c9bbc61cc99d5c3e9c038d8115
// https://github.com/mortennobel/emscripten-cubes/blob/master/main.cpp

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include "Shader.h"

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

struct CCamera
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

struct CVertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

struct CMesh
{
	std::vector<CVertex> Vertices;
	std::vector<uint32_t> Indices;
	// std::vector<GLuint> Textures;
};

struct CRenderObject
{
	uint32_t VAO;
	uint32_t VBO;
	uint32_t EBO;
};

struct CActor
{
	CMesh Mesh;
	glm::vec3 Position;
	glm::vec3 Rotation;
	glm::vec3 Scale;

	CRenderObject RenderObject;
};

namespace KeyCallbacks
{
	void MouseCameraCallback(
		const float& InX, 
		const float& InY, 
		CCamera& InCamera
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
		CCamera& InCamera
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
		if(InKey == SDLK_e)
		{
			InCamera.Position += InCamera.CameraSpeed * InCamera.CameraUp;
		}
		if(InKey == SDLK_q)
		{
			InCamera.Position -= InCamera.CameraSpeed * InCamera.CameraUp;
		}
	}
}

namespace Shapes
{
	CActor CreateCube()
	{
		CActor Object;

		Object.Mesh.Vertices = {
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

		Object.Mesh.Indices = {
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

		glGenVertexArrays(1, &Object.RenderObject.VAO);  // Vertex Array Object
		glGenBuffers(1, &Object.RenderObject.VBO);  // Vertex Buffer Object
		glGenBuffers(1, &Object.RenderObject.EBO);  // Element Buffer Object

		glBindVertexArray(Object.RenderObject.VAO);
		glBindBuffer(GL_ARRAY_BUFFER, Object.RenderObject.VBO);

		glBufferData(
			GL_ARRAY_BUFFER, 
			Object.Mesh.Vertices.size() * sizeof(CVertex), 
			&Object.Mesh.Vertices[0], 
			GL_STATIC_DRAW
		);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Object.RenderObject.EBO);
		glBufferData(
			GL_ELEMENT_ARRAY_BUFFER, 
			Object.Mesh.Indices.size() * sizeof(uint32_t), 
			&Object.Mesh.Indices[0], 
			GL_STATIC_DRAW
		);

		// Position attribute
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(
			0, 
			3, 
			GL_FLOAT, 
			GL_FALSE, 
			sizeof(CVertex), 
			(void*)0
		);
		// Normal attribute
		glVertexAttribPointer(
			1, 
			3, 
			GL_FLOAT, 
			GL_FALSE, 
			sizeof(CVertex), 
			(void*)offsetof(CVertex, Normal)
		);
		// Texture Coordinate attribute
		glVertexAttribPointer(
			2, 
			2, 
			GL_FLOAT, 
			GL_FALSE, 
			sizeof(CVertex), 
			(void*)offsetof(CVertex, TexCoords)
		);
		
		glBindVertexArray(0);

		return Object;
	}
}

class CRenderData
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

class CInputActions
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

struct CSdlContext
{
	SDL_Window* Window;
	SDL_GLContext Context;
};

CShader InitShaders()
{
	CShader Shader(
		"./Source/WSandBox/Shaders/V_Main.glsl",
		"./Source/WSandBox/Shaders/F_Main.glsl"
	);

	return Shader;
}

void Render(
	SDL_Window* InWindow, 
	CCamera& InCamera, 
	std::vector<CActor>& InActors, 
	CShader& InShader
)
{
	// Filled in later
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);

	glm::mat4 View = glm::mat4(1.f);
	View = glm::lookAt(
		InCamera.Position, 
		InCamera.Position + InCamera.CameraFront, 
		InCamera.CameraUp
	);

	glm::mat4 Projection = glm::mat4(1.f);
	Projection = glm::perspective(
		glm::radians(InCamera.FOV), 
		(float)CRenderData::SRC_WIDTH / (float)CRenderData::SRC_HEIGHT, 
		0.1f, 
		100.f
	);

	for(auto& Actor : InActors)
	{
		glUseProgram(InShader.ID);

		glm::mat4 Model = glm::mat4(1.f);
		Model = glm::translate(Model, Actor.Position);
		Model = glm::rotate(Model, glm::radians(Actor.Rotation.x), glm::vec3(1.f, 0.f, 0.f));
		Model = glm::rotate(Model, glm::radians(Actor.Rotation.y), glm::vec3(0.f, 1.f, 0.f));
		Model = glm::rotate(Model, glm::radians(Actor.Rotation.z), glm::vec3(0.f, 0.f, 1.f));
		Model = glm::scale(Model, Actor.Scale);

		glUniformMatrix4fv(
			glGetUniformLocation(InShader.ID, "Model"), 
			1, 
			GL_FALSE, 
			glm::value_ptr(Model)
		);
		glUniformMatrix4fv(
			glGetUniformLocation(InShader.ID, "View"), 
			1, 
			GL_FALSE, 
			glm::value_ptr(View)
		);
		glUniformMatrix4fv(
			glGetUniformLocation(InShader.ID, "Projection"), 
			1, 
			GL_FALSE, 
			glm::value_ptr(Projection)
		);

		glBindVertexArray(Actor.RenderObject.VAO);
		glDrawElements(
			GL_TRIANGLES, 
			Actor.Mesh.Indices.size(), 
			GL_UNSIGNED_INT, 
			0
		);

		glBindVertexArray(0);
	}

	SDL_GL_SwapWindow(InWindow);
}

void FreeResources()
{
	glDeleteProgram(program);
}

void MainLoop(
	SDL_Window* InWindow, 
	const CInputActions& InInputActions,
	CCamera& InCamera,
	std::vector<CActor>& InActors,
	CShader& InShader
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
					break;

				case SDL_MOUSEMOTION:
					InInputActions.RunMouseActions(event);
					break;
			}
		}
		Render(
			InWindow,
			InCamera,
			InActors,
			InShader
		);
	}
}

CSdlContext InitSDL()
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

std::vector<CActor> InitLevelActors()
{
	std::vector<CActor> Actors;

	for(glm::vec3 Position : {
			glm::vec3{0.f, 0.f, 0.f}, 
			glm::vec3{2.f, 5.f, -15.f}, 
			glm::vec3{-1.5f, -2.2f, -2.5f}, 
			glm::vec3{-3.8f, -2.0f, -12.3f}, 
			glm::vec3{2.4f, -0.4f, -3.5f}, 
			glm::vec3{-1.7f, 3.0f, -7.5f}, 
			glm::vec3{1.3f, -2.0f, -2.5f}, 
			glm::vec3{1.5f, 2.0f, -2.5f}, 
			glm::vec3{1.5f, 0.2f, -1.5f}, 
			glm::vec3{-1.3f, 1.0f, -1.5f}
		}
	)
	{
		CActor Actor = Shapes::CreateCube();

		Actor.Position = Position;
		Actor.Rotation = glm::vec3(0.f, 0.f, 0.f);
		Actor.Scale = glm::vec3(1.f, 1.f, 1.f);

		Actors.push_back(Actor);
	}

	return Actors;
}

int main(int argc, char* argv[])
{
	CSdlContext SdlContext = InitSDL();

	std::vector<CActor> Actors = InitLevelActors();
	CShader Shader = InitShaders();
	CCamera Camera;
	CInputActions InputActions;

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

	MainLoop(
		SdlContext.Window, 
		InputActions,
		Camera,
		Actors,
		Shader
	);

	FreeResources();

	SDL_GL_DeleteContext(SdlContext.Context);
	SDL_DestroyWindow(SdlContext.Window);
	SDL_Quit();

	return EXIT_SUCCESS;
}
