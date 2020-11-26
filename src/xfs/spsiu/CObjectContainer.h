#pragma once
#include <Windows.h>
#include <list>
#include "CSession.h"
#include <XFSSPI.H>
#include "CServiceProvider.h"

using namespace std;

class CObjectContainer
{
public:
	static bool addSession(CSession* session);
	static CSession* findSession(HSERVICE hService);
	static CSession* removeSession(HSERVICE hService);
	static CServiceProvider* getSP();
	static void clearSP();

private:
	typedef list<CSession*> SessionList;
	static SessionList sessions;

	typedef CServiceProvider* LPCServiceProvider;
	static LPCServiceProvider sp;
};