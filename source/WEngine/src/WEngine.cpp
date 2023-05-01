#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <memory>

#include "WEngine.h"

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

	if (TextureImage[0] = IMG_Load(filename))
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
	

}
int main(int argc, char** argv)
{
}