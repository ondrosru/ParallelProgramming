#include "LogFileWriter.h"

LogFileWriter::LogFileWriter( std::string fileName ): m_ofstream(fileName) {
}

void LogFileWriter::Write( std::string str ) {
	m_ofstream << str << std::endl;
}
