#include "CCommand.h"
#include <XFSSIU.h>

CCommand::CCommand(REQUESTID reqId, HSERVICE hService, HWND hWnd, int spiFunction, long timeout, DWORD command, LPVOID buffer, int size)
{
	this->m_reqId = reqId;
	this->m_hService = hService;
	this->m_hWnd = hWnd;
	this->m_dTimeout = timeout;
	this->m_Command = command;
	this->m_spiFunction = spiFunction;	
	this->m_result = NULL;
	this->m_EventClass = 0;
	this->m_hWndReg = NULL;
	this->m_session = NULL;
	this->m_TraceLevel = 0;

	copyInputBuffer(m_spiFunction, m_Command, buffer);
}

CCommand::CCommand(REQUESTID reqId, HSERVICE hService, HWND hWnd, int spiFunction, long timeout, DWORD dEventClass, HWND hWndReg, DWORD command, LPVOID buffer, int size, CSession* session)
{
	this->m_reqId = reqId;
	this->m_hService = hService;
	this->m_hWnd = hWnd;
	this->m_dTimeout = timeout;
	this->m_Command = command;
	this->m_spiFunction = spiFunction;		
	this->m_EventClass = dEventClass;
	this->m_hWndReg = hWndReg;
	this->m_session = session;
	this->m_result = NULL;
	this->m_TraceLevel = 0;

	copyInputBuffer(m_spiFunction, m_Command, buffer);
}

CCommand::~CCommand()
{
	freeBuffer();
}

void CCommand::freeBuffer() {
	if (m_buffer != NULL) {
		free(m_buffer);
		m_buffer = NULL;
	}

	if (m_BufferLszExtra != NULL) {
		free(m_BufferLszExtra);
		m_BufferLszExtra = NULL;
	}
}

void CCommand::copyInputBuffer(int spiFunction, DWORD command, LPVOID in_buffer) {
	//this->m_buffer = buffer;
	//this->m_bufferSize = size;
	//this->m_BufferLszExtra = NULL;

	m_buffer = NULL;
	m_bufferSize = 0;

	if (in_buffer == NULL)
		return;

	switch (spiFunction) {
		case WFS_EXECUTE_COMPLETE:
			switch (command) {
				case WFS_CMD_SIU_ENABLE_EVENTS:
				{
					m_bufferSize	= sizeof(WFSSIUENABLE);
					m_buffer		= malloc(m_bufferSize);

					if (m_buffer != NULL)
						memcpy(m_buffer, in_buffer, m_bufferSize);
				}
				break;
				case WFS_CMD_SIU_SET_DOOR:
				{
					m_bufferSize	= sizeof(WFSSIUSETDOOR);
					m_buffer		= malloc(m_bufferSize);

					if (m_buffer != NULL)
						memcpy(m_buffer, in_buffer, m_bufferSize);
				}
				break;
				case WFS_CMD_SIU_SET_INDICATOR:
				{
					m_bufferSize = sizeof(WFSSIUSETINDICATOR);
					m_buffer = malloc(m_bufferSize);

					if (m_buffer != NULL)
						memcpy(m_buffer, in_buffer, m_bufferSize);
				}
				break;
				case WFS_CMD_SIU_SET_AUXILIARY:
				{
					m_bufferSize = sizeof(WFSSIUSETAUXILIARY);
					m_buffer = malloc(m_bufferSize);

					if (m_buffer != NULL)
						memcpy(m_buffer, in_buffer, m_bufferSize);
				}
				break;
				case WFS_CMD_SIU_SET_GUIDLIGHT:
				{
					m_bufferSize = sizeof(WFSSIUSETGUIDLIGHT);
					m_buffer = malloc(m_bufferSize);

					if (m_buffer != NULL)
						memcpy(m_buffer, in_buffer, m_bufferSize);
				}
				break;
				case WFS_CMD_SIU_POWER_SAVE_CONTROL:
				{
					m_bufferSize = sizeof(WFSSIUPOWERSAVECONTROL);
					m_buffer = malloc(m_bufferSize);

					if (m_buffer != NULL)
						memcpy(m_buffer, in_buffer, m_bufferSize);
				}
				break;
				case WFS_CMD_SIU_SET_PORTS:
				{
					m_bufferSize = sizeof(WFSSIUSETPORTS);
					m_buffer = malloc(m_bufferSize);

					if (m_buffer != NULL)
						memcpy(m_buffer, in_buffer, m_bufferSize);

					if (((LPWFSSIUSETPORTS)in_buffer)->lpszExtra == NULL) {
						m_BufferLszExtra = NULL;
					}
					else {
						//Calcular o numero de caracteres do lpszExtra
						int lpszExtraSize = getLpszExtraSize(((LPWFSSIUSETPORTS)in_buffer)->lpszExtra);

						m_BufferLszExtra = (LPSTR)malloc(lpszExtraSize);

						if (m_BufferLszExtra != NULL)
							memcpy(m_BufferLszExtra, ((LPWFSSIUSETPORTS)in_buffer)->lpszExtra, lpszExtraSize);

						((LPWFSSIUSETPORTS)m_buffer)->lpszExtra = m_BufferLszExtra;
					}
				}
				break;
			}
	}
}

unsigned int CCommand::getLpszExtraSize(const char* lpszExtra) {
	bool cont = true;
	int idx = 0;

	while (cont) {
		//Verifica se a poisição atual E a próxima contém "\0"
		if ((lpszExtra[idx] == '\0') && (lpszExtra[idx + 1] == '\0')) {
			cont = false;
		}

		idx++;
	}

	//Retorn o numero de caracteres da estrutura
	//o numero inclue os marcadores de fim de dados ("\0\0")
	return idx + 1;
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
