
#include <iostream>
#include <string>
#include "helixlib/ReadFile.h"
// #include "emscripten/emscripten.h"


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


int main(int count, char** params)
{
	
	FILE *file = fopen("test/hello_workd_file.txt", "rb");
	if (!file)
	{
		printf("Cannot open file\n.");
		return 1;
	}

	while(!feof(file))
	{
		char c = fgetc(file);
		if (c != EOF)
		{
			putchar(c);
		}
	}

	fclose(file);
	MyFunction(MyStruct<int>());
	
	return 0;

}
