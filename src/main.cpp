
#include <iostream>
#include <string>
// #include "helixlib/ReadFile.h"


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
	
	MyFunction(MyStruct<int>());
	
	printf("Hello World\n");
	
	return 0;

}
