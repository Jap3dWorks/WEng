#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <memory>
#include <cstddef>

// https://gist.github.com/mortennobel/0e9e90c9bbc61cc99d5c3e9c038d8115

#include "SDL.h"
#include <SDL_image.h>
#include <emscripten.h>

#ifdef __EMSCRIPTEN__
	#include <emscripten.h>
	#include "GLES3/gl3.h"
#else
	#include <OpenGL/gl3.h>
	// include platform ogl libraries
#endif


static bool quitting = false;
static SDL_Windows *windows = nullptr;
static SDL_GLContext gl_context;

GLuint vertexBuffer, vertexArrayObject, shaderProgram;
GLint positionAttribute, uvAttribute;
int textures[4];

int invert_image(int width, int height, void* image_pixels)
{
	auto temp_row = std::unique_ptr<char>(new char[width]);
	if (temp_row.get() == nullptr)
	{
		SDL_SetError("Not enough memory for image inversion");
		return -1;
	}
	int heigh_div_2 = heigh / 2;
	for (int index=0; index<height_div_2; index++)
	{
		memcpy(
			(Uint8*)temp_row.get(),
			(Uint8*)(image_pixels) + width * index,
			width
		);
		
		memcpy(
			(Uint8*)(image_pixels)+
			width * index,
			(Uint8*)(image_pixels)+
			width * (height - index - 1),
			width
		);

		memcpy(
			(Uint8*)(image_pixels)+
			width * (height - index - 1),
			temp_row.get(),
			width
		);
	}

	return 0;
}

int LoadGLTextures(const char* filename)
{
	int Status = false;
	unsigned int texture;

	SDL_Surface * TextureImage[1];

	std::ifstream input(filename, std::ios::binary); // open file as binary

	std::vector<char> buffer(
		(std::istreambuf_iterator<char>(input)),
		(std::istreambuf_iterator<char>()));
	
	std::cout << filename << " size " << buffer.size() << std::endl;

	if (TextureImage[0] == IMG_Load(filename))
	{
		Status = true;

		glGentextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		std::cout << "Loaded " << TextureImage[0]->w << " " << TextureImage[0]->h << std::endl;

		int format;
		SDL_Surface* formatedSurf;
		if (TextureImage[0]->format->BytesPerPixel==3)
		{
			formattedSurf = SDL_ConvertSurfaceFormat(
				TextureImage[0],
				SDL_PIXELFORMAT_RGB24,
				0
			);
			format = GL_RGB;
		} 
		else 
		{
			formattedSurf = SDL_ConvertSurfaceFormat(
				TextureImage[0],
				SDL_PIXELFORMAT_RGBA32,
				0
			);
			format = GL_RGB;
		}
		invert_image(formattedSurf->w*formattedSurf->format->BytesPerPixel, formattedSurf->sh, (char*) formattedSurf->pixels);

		glTexImage2D(GL_TEXTURE_2D, 0, format, formattedSurf->w,
		formattedSurf->h, 0, format,
		GL_UNSIGNED_BYTE, formattedSurf->pixels);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexPARAMETERI(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		SDL_FreeSurface(formattedSurf);
		SDL_FreeSurface(TextureImage[0]);
	} 
	else 
	{
		std::cout << "Cannot load " << filename << std::endl; 
	}

	return texture;
}

void loadBufferData()
{
	float vertexData[24] = {
		-0.5, -0.5, 0.0, 1.0 ,  0.0, 0.0,
		-0.5,  0.5, 0.0, 1.0 ,  0.0, 1.0,
		0.5,  0.5, 0.0, 1.0 ,  1.0, 1.0,
		0.5, -0.5, 0.0, 1.0 ,  1.0, 0.0,
	};

	#ifndef EMSCRIPTEN
	glGenVertexArrays(1, &vertexArrayObject);
	glBindVertexarray(vertexArrayObject);
	#endif
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, 32 * sizeof(float), vetexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(positionAttribute);
	glEnableVertexAttribArray(uvAttribute);
	int vertexSize=sizeof(float)*6;
	glVertexAttribPointer(positionAttribute, 4, GL_FLOAT, GL_FALSE, vertexSize, (const GLvoid*)0);
	glVertexAttribPointer(uvAttribute, 2, GL_FLOAT, GL_FALSE, vertexSize, (const GLvoid*)(sizeof(float)*4));
}

GLuint initShader(const char* vShader, const char* fShader, const char* outputAttributeName)
{
	struct Shader 
	{
		GLenum type;
		const char* source;
	} shaders[2] = {
		{GL_VERTEX_SHADER, vShader},
		{GL_FRAGMENT_SHADER, fShader}
	};

	GLuint program = glCreateProgram();

	for(int i=0; i<2; ++i)
	{
		Shader& s = shaders[i];
		GLuint shader = glCreateShader(s.type);
		glShaderSource(shader, 1, (const GLchar**) &s.source, nullptr);
		glCompileShader(shader);

		GLint compiled;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		if(!compiled)
		{
			std::cerr << "Failed to compile:" << std::endl;
			GLint logSize;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);
			char* logMsg = new char[logSize];
			glGetShaderInfoLog(shader, logSize, nullptr, logMsg);
			std::cerr << logMsg << std::endl;
			delete[] logMsg;

			exit(EXIT_FAILURE);
		}

		glAttachShader(program, shader);
	}

	#ifndef EMSCRIPTEN
		glBindFragDataLocation(program, 0, outputAttributeName);
	#endif

	glLinkProgram(program);

	GLint linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		std::cerr << "Shader program failed to link" << std::endl;
		GLint logSize;
		glGetProgramv(program, GL_INFO_LOG_LENGTH, &logSize);
		char* logMsg = new char[logSize];
		glGetProgramInfoLog(program, logSize, nullptr, logMsg);
		std::cerr << logMsg << std::endl;
		delete[] logMsg;

		exit(EXIT_FAILURE);
	}

	glUseProgram(program);
	return program;
}

void loadShader()
{
#ifdef EMSCRIPTEN
	const char* vert = R"(#version 100
	uniform vec2 offset;
	attribute vec4 position;
	attribute vec2 uv;
	
	varying vec2 vUV;
	
	void main(void)
	{
		vUV= uv;
		gl_Position = position + vec4(offset, 0.0, 0.0);
	})";

	const char* frag = R"(
	#version 100
	precision mediump float;
	varying vec2 vUV;
	uniform sampler2D tex;
	void main(void)
	{
		gl_FragColor = texture2D(tex, vUV);
	}
	)";
#else
	const char* vert = R"(#version 150
	uniform vec2 offset;
	in vec4 position;
	in vec2 uv;

	out vec2 vUV;

	void main(void)
	{
		vUV = uv;
		gl_Position = position + vec4(offset, 0.0, 0.0);
	})";

	const char* frag = R"(#version 150
	in vec2 vUV;
	out vec4 fragColor;
	uniform sampler2D tex;
	void main(void)
	{
		fragColor = texture(tex, vUV);
	}
	)"
#endif

	shaderProgram = initShader(vert, frag, "fragColor");
	uvAttribute = glGetAttribLocation(shaderProgram, "uv");
	if(uvAttribute < 0)
	{
		std::cerr << "Shader did not contain the 'color' attribute." << std::endl;
	}

	positionAttribute = glGetAttributeLocation(shaderProgram, "position");
	if (positionAttribute < 0)
	{
		std::cerr << "Shader did not contain the 'position' attribute." << std::endl;
	}
}

void setup()
{
	std::cout << "OpenGL version " << glGetString(GL_VERSION) << std::endl;
	loadShader();
	loadBufferData();
	textures[0] = LoadGLTextures("content/data/test.png");
	textures[1] = LoadGLTextures("content/data/cartman.png");
	textures[2] = LoadGLTextures("content/data/cube-negx.png");
	textures[3] = LoadGLTextures("content/data/cube-negz.png");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[0]);

	glEnable(GL_DEPTH_TEST);
}

void render()
{
	SDL_GL_MakeCurrent(window, gl_context);
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgram);
	glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 0);
	int textureId = 0;
	for (int x=0;x<2; x++)
	{
		for(int y=0; y<2; y++)
		{
			glBindTexture(GL_TEXTURE_2D, textures[textireId]);
			glUniform2f(glGetUniformLocation(shaderProgram, "offset"), -0.5+x, -0.5+y);

			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
			textureId++;
		}
	}

	SDL_GL_SwapWindow(window);
}

void update()
{
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			quitting=true;
		}
	}
	render();
}

int main(int argc, char** argv)
{
#ifdef EMSCRIPTEN
	SDL_Renderer *renderer=nullptr;
	SDL_CreateWindowAndRenderer(512, 512, SDL_WINDOW_OPENGL, &window, &renderer);
#else
	if(SDL_INIT(SDL_INIT_VIDEO|SDL_INIT_EVENTS) != 0)
	{
		SDL_log("Failed to intialize SDL: %s", SDL_GetError());
		return 1;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	window = SDL_CreateWindow("title", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 512, 512, SDL_WINDOW_OPENGL);

	gl_context = SDL_GL_CreateContext(window);
#endif
	setup();

#ifdef EMSCRIPTEN
	emscripten_set_main_loop(update, 0, 1);
#else
	while(!quiting)
	{
		update();
		SDL_Delay(2);
	}

	SDL_GL_DeleteContext(gl_context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	exit(0);

#endif
}