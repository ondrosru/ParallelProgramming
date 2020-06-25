#pragma once
#include <Windows.h>

class ITask
{
public:
	~ITask() = default;
	virtual void execute() = 0;
};

static DWORD WINAPI threadProc( CONST LPVOID lpParam ) {
	ITask* task = (ITask*)lpParam;
	task->execute();
	ExitThread( 0 );
}