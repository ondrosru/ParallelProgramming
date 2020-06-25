#pragma once
#include "EasyBMP/EasyBMP.h"
#include "ITask.h"

struct ThreadData
{
	BMP* inputImage;
	BMP* outputImage;
	int startRow;
	int endRow;
	int blurRadius;
	int threadNum;
};

class BlurTask: public ITask
{
public:
	BlurTask( ThreadData& data );
	void execute() override;
private:
	ThreadData& m_threadData;
};

