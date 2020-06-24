#pragma once
#include <Windows.h>
#include <string>
#include "List.h"
#include "LogFileWriter.h"

class LogBuffer
{
public:
	LogBuffer( LogFileWriter* logFileWriter );
	~LogBuffer();
	void AddLog( std::string str );
private:
	const size_t MAX_SIZE = 500;
	CRITICAL_SECTION m_criticalSection;
	HANDLE m_eventHandle;
	HANDLE m_threadHandle;
	LogFileWriter* m_logFileWriter;
	List<std::string> m_logs;
	static DWORD WINAPI LogSizeMonitoringThread( CONST LPVOID lpParam );
};

