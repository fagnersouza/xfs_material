#pragma once
#include<Windows.h>
#include<deque>
#include<vector>
#include<XFSSPI.H>
#include "CCommand.h"
#include "CThread.h"
#include "CMutex.h"
#include "CDeviceAccess.h"

using namespace std;

struct RegisteredWindow {	
	//ID unica do serviço sendo executado
	HSERVICE Service;

	//Tipo de evento registrado
	DWORD EventClass;

	//Janela que irá receber o evento
	HWND WndReg;
};

enum class EWaitState{WS_NOTSTARTED, WFS_RUNNING, WS_COMPLETED};

class CServiceProvider : public CThread
{
private:
	deque<CCommand*> m_wosaQueue;
	char m_logicalName[128];
	vector<RegisteredWindow*> m_registeredWindows;
	
	//mutext para organizar o acesso a estrutura de comandos
	CMutex* m_mutCommands;

	int m_numberOfCommandsExecuting;
	EWaitState m_waitCommandState;
	int m_waitCommmandReqId;
	CDeviceAccess* m_device;

public:
	CServiceProvider();
	HRESULT insertCommand(CCommand* cmd);
	CCommand* removeCommand();
	void setLogicalName(const char* logicalName);
	const char* getLogicalName();

	//Funções que resolvem os comandos WFP
	HRESULT wfpOpen(CCommand* cmd);
	HRESULT wfpClose(CCommand* cmd);
	HRESULT wfpCancel(CCommand* cmd);
	RegisteredWindow* findRegisteredWindowsByHandle(HWND Wnd);
	RegisteredWindow* findRegisteredWindowsByService(HSERVICE hService);
	HRESULT wfpRegister(CCommand* cmd);
	HRESULT wfpDeregister(CCommand* cmd);
	HRESULT wfpExecute(CCommand* cmd);
	HRESULT wfpGetInfo(CCommand* cmd);
	HRESULT wfpLock(CCommand* cmd);
	HRESULT wfpUnlock(CCommand* cmd);
	HRESULT wfpSetTraceLevel(CCommand* cmd);

	//Funções utilitárias / suporte
	HRESULT allocateBuffer(LPWFSRESULT* bufferPointer);
	void setCommonData(LPWFSRESULT result, CCommand* cmd);
	void postMessageToWindow(CCommand* cmd);
	void notifyEvent(HWND Wnd, HSERVICE hService, int typeEvent, int eventID, WORD wBuffer);

	//Funções de controle de encerramento das request
	void startWaitExection(int reqId);
	int getWaitExecutionReqId();
	EWaitState getWaitExecutionState();
	void setWaitExecutionState(EWaitState state);

	int getNumberOfCommandsExecuting();
	void addNumberOfCommandsExecuting();
	void substractNumberOfCommandsExecuting();

	void* run();
};

class CCommandExecuter : public CThread{
private:
	CServiceProvider* m_sp;
	CCommand* m_command;

public:
	CCommandExecuter(CServiceProvider* sp, CCommand* cmd);
	void* run();
	void start();
};

