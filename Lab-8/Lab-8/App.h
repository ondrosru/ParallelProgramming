#pragma once
#include "BlurTask.h"
#include "Pool.h"
#include <vector>
#include <string>
#include <filesystem>
#include <iostream>


class App
{
public:
	App();
	~App();
	void run( int argc, char* argv[] );
private:
	enum class ProccesingMode
	{
		BASIC,
		POOL
	};

	ProccesingMode parseProccesingMode( const char* str );
	std::vector<ThreadData> getThreadDatas( BMP* inputImage, BMP* outputImage, int blurRadius, int blockCount ) const;
};

