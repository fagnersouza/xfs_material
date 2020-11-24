#pragma once
#include <Windows.h>
#include <list>
#include "CSession.h"
#include <XFSSPI.H>

using namespace std;

class CObjectContainer
{
public:
	static bool addSession(CSession* session);
	static CSession* findSession(HSERVICE hService);

private:
	typedef list<CSession*> SessionList;
	static SessionList sessions;
};