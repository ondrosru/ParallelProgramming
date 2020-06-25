#include "App.h"
#include <iostream>

#include "BlurTask.h"

int main( int argc, char* argv[] ) {
	try {
		App().run( argc, argv );
	}
	catch( const std::exception& ex ) {
		std::cerr << ex.what() << std::endl;
		return -1;
	}
	return 0;
}