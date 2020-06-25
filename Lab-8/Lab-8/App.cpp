#include "App.h"
#include <ctime>

App::App() {
}

App::~App() {
}

void App::run( int argc, char* argv[] ) {
	if( argc == 2 && (std::strcmp( argv[1], "-h" ) == 0 || std::strcmp( argv[1], "-help" ) == 0) ) {
		std::cout << "Help:\n"
			<< "Command to run: Lab-8.exe <processing mode> <blur radius> <input directory> <output directory> <count blocks> <thread count>\n"
			<< "Arguments:\n"
			<< "\t <processing mode>:\n"
			<< "\t\t 0 - creating a new thread for each operation\n"
			<< "\t\t 1 - use pool\n"
			<< "\t <blur radius> - blur radius\n"
			<< "\t <input path> - directory with pictures for processing\n"
			<< "\t <output path> - directory for saving pictures\n"
			<< "\t <count blocks> - number of blocks for splitting a picture\n"
			<< "\t <thread count> - number of threads per pool\n";
		return;
	}

	if( argc != 7 ) {
		std::string error = "Error command!!!\n";
		error += "Command to run: Lab-8.exe <processing mode> <blur radius> <input directory> <output directory> <count blocks> <thread count>\n";
		error += "Help command: Lab-8.exe -h\n";
		throw std::invalid_argument( error );
	}

	ProccesingMode proccesingMode = parseProccesingMode( argv[1] );
	int blurRadius = std::stoi( argv[2] );
	std::string inputDirectory = argv[3];
	std::string outputDirectory = argv[4];
	int countBlocks = std::stoi( argv[5] );
	int threadCount = std::stoi( argv[6] );

	if( !std::filesystem::exists( inputDirectory ) ) {
		throw std::exception( "This directory does not exist" );
	}

	if( !std::filesystem::exists( outputDirectory ) ) {
		std::filesystem::create_directories( outputDirectory );
	}

	std::vector<std::string> inputImagesPaths;
	std::vector<std::string> outputImagesPaths;
	for( const auto& file : std::filesystem::directory_iterator( inputDirectory ) ) {
		auto path = file.path();

		if( path.extension() == ".bmp" ) {
			inputImagesPaths.push_back( path.string() );
			outputImagesPaths.push_back( outputDirectory + "/" + "b_" + path.stem().string() + ".bmp" );
		}
	}

	std::clock_t start = std::clock();
	for( size_t i = 0; i < inputImagesPaths.size(); i++ ) {
		
		BMP inputImage;
		if( !inputImage.ReadFromFile( inputImagesPaths[i].c_str() ) ) {
			std::string errorMsg = "Failed to read file: ";
			errorMsg += inputImagesPaths[i];
			throw std::exception( errorMsg.c_str() );
		}
		BMP outputImage(inputImage);
		std::vector<ThreadData> threadDatas = getThreadDatas(&inputImage, &outputImage, blurRadius, countBlocks);

		std::vector<ITask*> tasks;

		for( ThreadData& threadData: threadDatas ) {
			tasks.push_back( new BlurTask( threadData ) );
		}

		if( proccesingMode == ProccesingMode::BASIC ) {
			std::vector<HANDLE> handles(tasks.size());
			for( size_t j = 0; j < tasks.size(); ++j ) {
				handles[j] = CreateThread( nullptr, 0, &threadProc, tasks[j], CREATE_SUSPENDED, nullptr );
			}

			for( const auto& handle: handles) {
				ResumeThread( handle );
			}

			WaitForMultipleObjects( (DWORD)tasks.size(), handles.data(), true, INFINITE );
		}
		else if( proccesingMode == ProccesingMode::POOL ) {
			Pool pool( tasks, threadCount );
			pool.execute();
		}

		outputImage.WriteToFile( outputImagesPaths[i].c_str() );
	}
	std::cout << "Runtime: " << clock() - start << " ms" << std::endl;
}

App::ProccesingMode App::parseProccesingMode( const char* str ) {
	if( std::strcmp( str, "0" ) == 0 ) {
		return ProccesingMode::BASIC;
	}
	else if( std::strcmp( str, "1" ) == 0 ) {
		return ProccesingMode::POOL;
	}
	else {
		throw std::invalid_argument( "Invalid argument: " + std::string( str ) );
	}
}

std::vector<ThreadData> App::getThreadDatas( BMP* inputImage, BMP* outputImage, int blurRadius, int blockCount ) const {
	std::vector<ThreadData> threadDatas;
	int height = inputImage->TellHeight();
	div_t divider = div( height, blockCount );

	int startRow = 0;
	int endRow = divider.quot;
	int rem = divider.rem;
	for( size_t i = 0; i < blockCount; i++ ) {
		if( rem > 0 ) {
			endRow++;
			rem--;
		}
		

		ThreadData threadData;
		threadData.inputImage = inputImage;
		threadData.outputImage = outputImage;
		threadData.blurRadius = blurRadius;
		threadData.threadNum = (int)i;
		threadData.startRow = startRow;
		threadData.endRow = endRow;

		threadDatas.push_back( threadData );

		startRow = endRow;
		endRow += divider.quot;
	}

	return threadDatas;
}
