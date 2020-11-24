#include "CSession.h"

CSession::CSession(const char* logicalName, HSERVICE hService)
{	
	this->m_logicalName		= _strdup(logicalName);
	this->m_hService		= hService;
}

HSERVICE CSession::getHService()
{
	return this->m_hService;
}

const char* CSession::getLogicalName()
{
	return this->m_logicalName;
}
