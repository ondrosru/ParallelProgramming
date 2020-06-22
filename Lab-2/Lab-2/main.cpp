#include <iostream>
#include <Windows.h>
#include <math.h>
#include <ctime>
#include "EasyBMP.h"

const int BLUR_RANGE_SIZE = 5;

struct ThreadData
{
	BMP* inputBmp;
	BMP* outputBmp;
	int startRow;
	int endRow;
	int rangeSize;
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
		}
	}
	ExitThread( 0 );
}

void Blur(char* inputImgName, char* outputImageName, int threadCount, int coresCount ) {
	BMP inputImage;
	inputImage.ReadFromFile( inputImgName );
	BMP outputImage;
	outputImage.SetSize( inputImage.TellWidth(), inputImage.TellHeight() );
	ThreadData* threadData = new ThreadData[threadCount];
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

		threadData[i] = { &inputImage, &outputImage, startRow, endRow, BLUR_RANGE_SIZE };

		startRow = endRow + 1;
		endRow += divider.quot;
	}

	int mask = (1 << coresCount) - 1;
	HANDLE* handles = new HANDLE[threadCount];
	for ( int i = 0; i < threadCount; i++ )
	{
		handles[i] = CreateThread( NULL, 0, &ThreadProc, &threadData[i], CREATE_SUSPENDED, NULL );
		SetThreadAffinityMask( handles[i], mask );
	}

	for ( int i = 0; i < threadCount; i++ )
	{
		ResumeThread( handles[i] );
	}

	WaitForMultipleObjects( threadCount, handles, true, INFINITE );
	outputImage.WriteToFile( outputImageName );
	delete[] threadData;
	delete[] handles;
}



int main(int argc, char* argv[]) {

	
	if ( argc != 5 )
	{
		std::cout << "Invalid command line arguments.\n Example: Lab-2.exe <input file name> <output file name> <threads count> <cores count>" << std::endl;
	}
	std::clock_t start_time = std::clock();
	char* inputImageName = argv[1];
	char* outputImageName = argv[2];
	int threadCount = atoi( argv[3] );
	int coresCount = atoi( argv[4] );
	Blur( inputImageName, outputImageName, threadCount, coresCount );
	std::clock_t end_time = std::clock();
	std::cout << end_time - start_time << " ms" << std::endl;
	return 0;
}