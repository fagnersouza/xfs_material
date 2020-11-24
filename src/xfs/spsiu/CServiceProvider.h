#pragma once
#include<Windows.h>
#include<XFSSPI.H>
#include "CCommand.h"
#include "CThread.h"
#include<deque>

using namespace std;

class CServiceProvider : public CThread
{
private:
	deque<CCommand*> m_wosaQueue;
	char m_logicalName[128];

public:
	CServiceProvider();
	HRESULT insertCommand(CCommand* cmd);
	CCommand* removeCommand();
	void setLogicalName(const char* logicalName);
	const char* getLogicalName();

	//Funções que resolvem os comandos WFP
	HRESULT wfpOpen(CCommand* cmd);
	HRESULT wfpClose(CCommand* cmd);
	HRESULT wfpRegister(CCommand* cmd);
	HRESULT wfpDeregister(CCommand* cmd);
	HRESULT wfpExecute(CCommand* cmd);
	HRESULT wfpGetInfo(CCommand* cmd);
	HRESULT wfpLock(CCommand* cmd);
	HRESULT wfpUnlock(CCommand* cmd);

	//Funções utilitárias / suporte
	HRESULT allocateBuffer(LPWFSRESULT* bufferPointer);
	void setCommonData(LPWFSRESULT result, CCommand* cmd);
	void postMessageToWindow(CCommand* cmd);

	void* run();
};

class CCommandExecuter : public CThread{
private:
	CServiceProvider* m_sp;
	CCommand* m_command;

public:
	CCommandExecuter(CServiceProvider* sp, CCommand* cmd);
	void* run();
};

