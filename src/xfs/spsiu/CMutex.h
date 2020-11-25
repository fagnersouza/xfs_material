#pragma once
#include <Windows.h>

class CMutex
{
private:
	bool locked;
	HANDLE ghMutex;

public:
	CMutex();

	void request();
	void release();
	bool isLocked();
};

