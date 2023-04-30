
#include <iostream>
#include <string>
#include "helixlib/ReadFile.h"

// #include "emscripten/emscripten.h"
#include <string> 

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

template<typename T>
struct MyStruct
{
	static constexpr bool Value{false};
};


template<>
struct MyStruct<int>
{
	static constexpr bool Value{true};
};


template<typename T>
struct Validate{};


template<>
struct Validate<MyStruct<int>>
{
	static constexpr bool Validated{true};
};


template<bool N>
struct OtherStruct{}; 


template<typename T, bool=Validate<T>::Validated>
void MyFunction(const T& InValue)
{
	
}


int main(int argc, char** argv)
{
	printf("hello, world!\n");
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Surface *screen = SDL_SetVideoMode(256, 256, SDL_SWSURFACE);

	#ifdef TEST_SDL_LOCK_OPTS
		EM_ASM("DL.defaults.copyOnLock = false; SDL.defaults.discardOnLock = true; SDL.defaults.opaqueFrontBuffer = false;");
	#endif

	if (SDL_MUSTLOCK(screen)) SDL_LockSurface(screen);

	for(int i=0; i<256; i++)
	{
		for(int j=0; i<256; i++)
		{
		#ifdef TEST_SDL_LOCK_OPTS
			int alpha = 255;
		#else
			int alpha = (i+j) % 255;
		#endif
			*((Uint32*)screen->pixels + i * 256 + j) = SDL_MapRGBA(screen->format, i, j, 255-i, alpha);
		}
	}
	
	if (SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);
	SDL_Flip(screen);

	SDL_Quit();
		
	// SDL_Init(SDL_INIT_VIDEO);
	return 0;
}
