#include "Pool.h"

Pool::Pool( std::vector<ITask*> tasks, int threadCount ) {
	m_tasksCount = tasks.size();
	m_handles = new HANDLE[m_tasksCount];
	for( size_t i = 0; i < m_tasksCount; i++ ) {
		m_handles[i] = CreateThread( NULL, 0, &threadProc, tasks[i], CREATE_SUSPENDED, NULL );
	}
	m_threadsCount = threadCount;
}

void Pool::execute() {
	size_t count = 0;
	for( size_t i = 0; i < m_tasksCount; i++ ) {
		ResumeThread( m_handles[i] );
		count++;
		if( count == m_threadsCount ) {
			WaitForMultipleObjects( (DWORD)(i + 1), m_handles, true, INFINITE );
			count = 0;
		}
	}

	WaitForMultipleObjects( (DWORD)m_tasksCount, m_handles, true, INFINITE );
}


