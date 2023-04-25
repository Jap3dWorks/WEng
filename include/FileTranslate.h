#include <iostream>
#include <string>


class SRFile
{
private:
	std::string FilePath;

public:
   explicit SRReadFile(const std::string &InFilePath)
       : FilePath(InFilePath) {}

	SRReadFile()=delete;
	
	~SRReadFile()=default;
	
	SRReadFile(const SRReadFile& other)=default;
	SRReadFile(SRReadFile&& other)=default;

	SRReadFile& operator=(const SRReadFile& other)=default;
	SRReadFile& operator=(SRReadFile&& other)=default;
};


class SRFileMd5
{
	private:
	SRFile File;

	
}

