#include "LogBuffer.h"
#include <exception>

LogBuffer::LogBuffer( LogFileWriter* logFileWriter )
	:m_criticalSection(CRITICAL_SECTION()),
	m_logFileWriter(logFileWriter)
{
	if ( !InitializeCriticalSectionAndSpinCount( &m_criticalSection, 0x00000400 ) )
	{
		throw std::exception( "Critical section not initialize" );
	}

	m_eventHandle = CreateEvent( nullptr, TRUE, FALSE, TEXT( "Event" ) );
	m_threadHandle = CreateThread( NULL, 0, &LogSizeMonitoringThread, (void*)this, 0, NULL );
}


LogBuffer::~LogBuffer() {
	EnterCriticalSection( &m_criticalSection );
	SetEvent( m_eventHandle );
	DWORD wait = WaitForSingleObject( m_threadHandle, INFINITE );
	if ( wait == WAIT_OBJECT_0 )
	{
		ResetEvent( m_eventHandle );
		m_threadHandle = CreateThread( NULL, 0, &LogSizeMonitoringThread, (void*)this, 0, NULL );
	}
	LeaveCriticalSection( &m_criticalSection );
	if ( &m_criticalSection )
	{
		DeleteCriticalSection( &m_criticalSection );
	}
	CloseHandle( m_eventHandle );
	CloseHandle( m_threadHandle );
}

void LogBuffer::AddLog( std::string str ) {
	EnterCriticalSection( &m_criticalSection );
	m_logs.Add( str );
	if ( m_logs.Size() >= MAX_SIZE )
	{
		SetEvent( m_eventHandle );
		DWORD wait = WaitForSingleObject( m_threadHandle, INFINITE );
		if ( wait == WAIT_OBJECT_0 )
		{
			ResetEvent( m_eventHandle );
			m_threadHandle = CreateThread( NULL, 0, &LogSizeMonitoringThread, (void*)this, 0, NULL );
		}
	}
	LeaveCriticalSection( &m_criticalSection );
}

DWORD WINAPI LogBuffer::LogSizeMonitoringThread( CONST LPVOID lpParam ) {
	LogBuffer* data = (LogBuffer*)lpParam;
	
	if ( WaitForSingleObject( data->m_eventHandle, INFINITE ) == WAIT_OBJECT_0 )
	{
		for ( List<std::string>::Iterator it = data->m_logs.Begin(); it != data->m_logs.End(); ++it )
		{
			data->m_logFileWriter->Write( *it );
		}
		data->m_logs.Clear();
	}

	ExitThread(0);
}
