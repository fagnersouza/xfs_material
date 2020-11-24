#include "CThread.h"

void CThread::start()
{
	this->hThreadHandle = CreateThread(
										NULL,
										0,
										dispatch,
										this,
										0,
										&this->dThreadId
										);
}

bool CThread::stop()
{
	this->exit = true;
	return true;
}

bool CThread::join()
{
	WaitForSingleObject(this->hThreadHandle, INFINITE);
	return true;
}

bool CThread::isRunning()
{
	return this->running;
}

HANDLE CThread::getHandle()
{
	return this->hThreadHandle;
}

DWORD CThread::getID()
{
	return this->dThreadId;
}

bool CThread::isToExit()
{
	return this->exit;
}

DWORD CThread::dispatch(void* threadObj) {
	((CThread*)threadObj)->run();

	return 1;
}
