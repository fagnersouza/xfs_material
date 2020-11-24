#pragma once
#include<Windows.h>
#include <XFSSPI.H>

class CSession
{
private:
	HSERVICE m_hService;
	char* m_logicalName;

public:
	CSession(const char* logicalName, HSERVICE hService);
	HSERVICE getHService();
	const char* getLogicalName();
};

