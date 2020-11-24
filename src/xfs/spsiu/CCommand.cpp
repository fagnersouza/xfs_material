#include "CCommand.h"

CCommand::CCommand(REQUESTID reqId, HSERVICE hService, HWND hWnd, int spiFunction, long timeout, DWORD command, LPVOID buffer, int size)
{
	this->m_reqId = reqId;
	this->m_hService = hService;
	this->m_hWnd = hWnd;
	this->m_spiFunction = spiFunction;
	this->m_dTimeout = timeout;
	this->m_Command = command;
	this->m_buffer = buffer;
	this->m_bufferSize = size;
}

REQUESTID CCommand::getRedId()
{
	return this->m_reqId;
}

HSERVICE CCommand::getService()
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

void CCommand::setResult(LPVOID result)
{
	this->m_result = result;
}
