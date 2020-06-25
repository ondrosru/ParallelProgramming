#include "BlurTask.h"

BlurTask::BlurTask( ThreadData& data )
	: m_threadData(data)
{
}

void BlurTask::execute() {
	BMP* inputImage = m_threadData.inputImage;
	BMP* outputImage = m_threadData.outputImage;
	int size = m_threadData.blurRadius;
	int imageWidth = inputImage->TellWidth();
	int imageHeight = inputImage->TellHeight();
	for( int i = m_threadData.startRow; i < m_threadData.endRow; i++ ) {
		for( int j = 0; j < imageWidth; j++ ) {
			int sumRed = 0;
			int sumGreen = 0;
			int sumBlue = 0;
			int countPixel = 0;
			for( int x = -size; x <= size; x++ ) {
				for( int y = -size; y <= size; y++ ) {
					if( (i + y) < imageHeight && (i + y) >= 0 && (j + x) < imageWidth && (j + x) >= 0 ) {
						RGBApixel pixel = inputImage->GetPixel( j + x, i + y );
						sumRed += pixel.Red;
						sumGreen += pixel.Green;
						sumBlue += pixel.Blue;
						countPixel++;
					}
				}
			}

			RGBApixel pixel;
			pixel.Red = sumRed / countPixel;
			pixel.Green = sumGreen / countPixel;
			pixel.Blue = sumBlue / countPixel;
			pixel.Alpha = 1;
			outputImage->SetPixel( j, i, pixel );
		}
	}
}
