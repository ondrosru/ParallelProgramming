#include <iostream>
#include <Windows.h>
#include <math.h>
#include <ctime>
#include <fstream>
#include <vector>
#include <string>
#include "pch.h"
#include "EasyBMP.h"
#include "LogFileWriter.h"
#include "LogBuffer.h"

const int BLUR_RANGE_SIZE = 5;
std::clock_t _startTime;
std::string LOG_FILE_NAME = "logFile.txt";

struct ThreadData
{
	BMP* inputBmp;
	BMP* outputBmp;
	int startRow;
	int endRow;
	int rangeSize;
	int threadNum;
	LogBuffer* logBuffer;
};

DWORD WINAPI ThreadProc( CONST LPVOID lpParam ) {
	const ThreadData* data = static_cast<ThreadData*>(lpParam);
	BMP* inputImage = data->inputBmp;
	BMP* outputImage = data->outputBmp;
	int size = data->rangeSize;
	int startRow = data->startRow;
	int endRow = data->endRow;
	int width = inputImage->TellWidth();
	int height = inputImage->TellHeight();
	for ( int i = startRow; i <= endRow; i++ )
	{
		for ( int j = 0; j < width; j++ )
		{
			int sumR = 0;
			int sumG = 0;
			int sumB = 0;
			int count = 0;
			for ( int x = -size; x <= size; x++ )
			{
				for ( int y = -size; y <= size; y++ )
				{
					if ( (i + y) < height && (i + y) >= 0 && (j + x) < width && (j + x) >= 0 )
					{
						RGBApixel pixel = inputImage->GetPixel( j + x, i + y );
						sumR += pixel.Red;
						sumG += pixel.Green;
						sumB += pixel.Blue;
						count++;
					}
				}
			}
			RGBApixel pixel;
			pixel.Red = sumR / count;
			pixel.Green = sumG / count;
			pixel.Blue = sumB / count;
			pixel.Alpha = 1;
			outputImage->SetPixel( j, i, pixel );
			std::string str = std::to_string( data->threadNum ) + " " + std::to_string( clock() - _startTime );
			data->logBuffer->AddLog( str );
		}
	}
	ExitThread( 0 );
}

void Blur(char* inputImgName, char* outputImageName, int threadCount, int coresCount, std::vector<ThreadPriority> prioritiesForThreads ) {
	BMP inputImage;
	inputImage.ReadFromFile( inputImgName );
	BMP outputImage;
	outputImage.SetSize( inputImage.TellWidth(), inputImage.TellHeight() );
	ThreadData* threadData = new ThreadData[threadCount];
	std::ofstream* outputFilesStreams = new std::ofstream[threadCount];

	LogFileWriter* fileWriter = new LogFileWriter(LOG_FILE_NAME);
	LogBuffer* logBuffer = new LogBuffer(fileWriter);

	std::div_t divider	= std::div(inputImage.TellHeight(), threadCount);
	int startRow = 0;
	int endRow = divider.quot - 1;
	int remainder = divider.rem;
	for ( int i = 0; i < threadCount; i++ )
	{
		if ( remainder != 0 )
		{
			endRow++;
			remainder--;
		}
		int threadNum = i + 1;
		threadData[i] = { &inputImage, &outputImage, startRow, endRow, BLUR_RANGE_SIZE, threadNum, logBuffer };

		startRow = endRow + 1;
		endRow += divider.quot;
	}

	int mask = (1 << coresCount) - 1;
	HANDLE* handles = new HANDLE[threadCount];
	for ( int i = 0; i < threadCount; i++ )
	{
		handles[i] = CreateThread( NULL, 0, &ThreadProc, &threadData[i], CREATE_SUSPENDED, NULL );
		SetThreadAffinityMask( handles[i], mask );
		SetThreadPriority( handles[i], prioritiesForThreads[i] );
	}

	for ( int i = 0; i < threadCount; i++ )
	{
		ResumeThread( handles[i] );
	}

	WaitForMultipleObjects( threadCount, handles, true, INFINITE );
	outputImage.WriteToFile( outputImageName );
	delete logBuffer;
	delete[] threadData;
	delete[] handles;
}



int main(int argc, char* argv[]) {

	if ( argc == 2 && (std::strcmp( argv[1], "-h" ) == 0 || std::strcmp( argv[1], "-help" ) == 0) )
	{
		std::cout << "Example: Lab-4.exe <input file name> <output file name> <cores count> <threads count>";
		return 0;
	}

	if ( argc != 5 )
	{
		std::cout << "Invalid command line arguments.\n Example: Lab-4.exe <input file name> <output file name> <cores count> <threads count>" << std::endl;
		return 0;
	}

	char* inputImageName = argv[1];
	char* outputImageName = argv[2];
	int coresCount = atoi( argv[3] );
	int threadCount = atoi( argv[4] );
	if ( coresCount < 1 )
	{
		std::cerr << "Error: Number of cores less than 1" << std::endl;
		return -1;
	}

	if ( threadCount < 1 )
	{
		std::cerr << "Error: Number of threads less than 1" << std::endl;
		return -1;
	}
	std::vector<ThreadPriority> prioritiesForThreads;
	std::cout << "Priorities for threads:\n"
		<< "1. below_normal\n"
		<< "2. normal\n"
		<< "3. above_normal\n"
		<< std::endl;
	for ( int i = 0; i < threadCount; i++ )
	{
		std::cout << "Priority for thread num " << i + 1 << ": ";
		std::string str;
		std::cin >> str;
		bool flag = true;
		do {
			if ( (str == "below_normal") || (atoi( str.c_str() ) == 1) )
			{
				prioritiesForThreads.push_back( ThreadPriority::PRIORITY_BELOW_NORMAL );
				flag = false;
			}
			else if ( (str == "normal") || (atoi( str.c_str() ) == 2) )
			{
				prioritiesForThreads.push_back( ThreadPriority::PRIORITY_NORMAL );
				flag = false;
			}
			else if ( (str == "above_normal") || (atoi( str.c_str() ) == 3) )
			{
				prioritiesForThreads.push_back( ThreadPriority::PRIORITY_ABOVE_NORMAL );
				flag = false;
			}
			else
			{
				std::cout << "Priorities for threads:\n"
					<< "\t 1. below_normal\n"
					<< "\t 2. normal\n"
					<< "\t 3. above_normal\n"
					<< "Input example: 1\n"
					<< "Or: below_normal\n"
					<< std::endl;
			}
		} while ( flag );
		

	}
	_startTime = std::clock();
	Blur( inputImageName, outputImageName, threadCount, coresCount, prioritiesForThreads );
	return 0;
}