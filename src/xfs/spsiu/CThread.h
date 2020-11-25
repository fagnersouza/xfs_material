#pragma once
#include <Windows.h>

using namespace std;

class CThread
{
public:
	void start();
	bool stop();
	bool join();
	bool isRunning();
	HANDLE getHandle();
	DWORD getID();

protected:
	virtual void* run() = 0;
	bool isToExit();
	bool running = false;

private:
	HANDLE hThreadHandle;
	DWORD dThreadId;	
	bool exit = false;
	static DWORD FAR PASCAL dispatch(void* threadObj);
};

