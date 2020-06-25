#pragma once
#include "ITask.h"
#include <vector>

class Pool
{
public:
	Pool( std::vector<ITask*> tasks, int threadCount );
	void execute();
private:
	HANDLE* m_handles;
	size_t m_tasksCount;
	size_t m_threadsCount;
};

