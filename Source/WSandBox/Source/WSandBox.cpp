// https://gist.github.com/mortennobel/0e9e90c9bbc61cc99d5c3e9c038d8115
// https://github.com/mortennobel/emscripten-cubes/blob/master/main.cpp

#include <iostream>
#include <string>

#include <GL/glew.h>
#include <GL/glut.h>
#include <SDL2/SDL.h>

#include <chrono>
#include <memory>

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

GLuint program;
GLuint vbo_triangle;

bool init_resources(void)
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
		0.0,   0.8, 1.f, 0.f, 0.f, 
		-0.8, -0.8, 0.f, 1.f, 0.f,
		0.8,  -0.8, 1.f, 0.f, 1.f
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

void render(SDL_Window* window)
{
	// Filled in later
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	SDL_GL_SwapWindow(window);
}

void free_resources()
{
	glDeleteProgram(program);
	// Filled in later
}

void mainLoop(SDL_Window* window)
{
	while (true)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				return;
		}
		render(window);
	}
}

int main(int argc, char* argv[])
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
		return EXIT_FAILURE;
	}

	SDL_GLContext context = SDL_GL_CreateContext(window);

	GLenum glew_status = glewInit();

	if (glew_status != GLEW_OK)
	{
		std::cerr << "Error: " << glewGetErrorString(glew_status) << std::endl;
		return EXIT_FAILURE;
	}

	if (!init_resources())
		return EXIT_FAILURE;
	
	mainLoop(window);

	free_resources();

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return EXIT_SUCCESS;
}
