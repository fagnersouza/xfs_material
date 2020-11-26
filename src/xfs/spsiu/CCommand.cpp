#include "CCommand.h"

CCommand::CCommand(REQUESTID reqId, HSERVICE hService, HWND hWnd, int spiFunction, long timeout, DWORD command, LPVOID buffer, int size)
{
	this->m_reqId = reqId;
	this->m_hService = hService;
	this->m_hWnd = hWnd;
	this->m_dTimeout = timeout;
	this->m_Command = command;
	this->m_spiFunction = spiFunction;	
	this->m_buffer = buffer;
	this->m_bufferSize = size;
	this->m_result = NULL;
	this->m_BufferLszExtra = NULL;
	this->m_EventClass = 0;
	this->m_hWndReg = NULL;
	this->m_session = NULL;
}

CCommand::CCommand(REQUESTID reqId, HSERVICE hService, HWND hWnd, int spiFunction, long timeout, DWORD dEventClass, HWND hWndReg, DWORD command, LPVOID buffer, int size, CSession* session)
{
	this->m_reqId = reqId;
	this->m_hService = hService;
	this->m_hWnd = hWnd;
	this->m_dTimeout = timeout;
	this->m_Command = command;
	this->m_spiFunction = spiFunction;		
	this->m_buffer = buffer;
	this->m_bufferSize = size;
	this->m_EventClass = dEventClass;
	this->m_hWndReg = hWndReg;
	this->m_session = session;
	this->m_result = NULL;
	this->m_BufferLszExtra = NULL;
}

REQUESTID CCommand::getReqId()
{
	return this->m_reqId;
}

HSERVICE CCommand::getHService()
{
	return this->m_hService;
}

HWND CCommand::getHWND()
{
	return this->m_hWnd;
}

DWORD CCommand::getTimeout()
{
	return this->m_dTimeout;
}

DWORD CCommand::getCommand()
{
	return this->m_Command;
}

int CCommand::getSPIFunction()
{
	return this->m_spiFunction;
}

LPVOID CCommand::getBuffer()
{
	return this->m_buffer;
}

int CCommand::getBufferSize()
{
	return this->m_bufferSize;
}

LPVOID CCommand::getResult()
{
	return this->m_result;
}

char* CCommand::getLpszExtra()
{
	return this->m_BufferLszExtra;
}

DWORD CCommand::getEventClass()
{
	return this->m_EventClass;
}

HWND CCommand::getWndReg()
{
	return this->m_hWndReg;
}

CSession* CCommand::getSession()
{
	return this->m_session;
}

void CCommand::setResult(LPVOID result)
{
	this->m_result = result;
}

void CCommand::setTraceLevel(DWORD dwTraceLevel)
{
	this->m_TraceLevel = dwTraceLevel;
}

DWORD CCommand::getTraceLevel()
{
	return this->m_TraceLevel;
}
