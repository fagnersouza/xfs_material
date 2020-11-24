#include "CServiceProvider.h"
#include "CSession.h"
#include "CObjectContainer.h"

CServiceProvider::CServiceProvider()
{
    
}

HRESULT CServiceProvider::insertCommand(CCommand* cmd)
{
    HRESULT hResult = WFS_ERR_INTERNAL_ERROR;

    if (cmd != NULL) {
        //TODO: implementar Mutex
        m_wosaQueue.push_back(cmd);
        //TODO
        hResult = WFS_SUCCESS;
    }

    return hResult;
}

CCommand* CServiceProvider::removeCommand()
{
    CCommand* command = NULL;
    
    if (m_wosaQueue.size() > 0) {
        command = m_wosaQueue.at(0);
        m_wosaQueue.pop_front();
    }
    
    return command;
}

void CServiceProvider::setLogicalName(const char* logicalName)
{
    strcpy_s(m_logicalName, logicalName);
}

const char* CServiceProvider::getLogicalName() {
    return this->m_logicalName;
}

HRESULT CServiceProvider::wfpOpen(CCommand* cmd)
{
    CSession* session = new CSession(this->getLogicalName(), cmd->getService());

    CObjectContainer::addSession(session);
    
    return WFS_SUCCESS;
}

HRESULT CServiceProvider::wfpClose(CCommand* cmd)
{
    return WFS_ERR_INTERNAL_ERROR;
}

HRESULT CServiceProvider::wfpRegister(CCommand* cmd)
{
    return WFS_ERR_INTERNAL_ERROR;
}

HRESULT CServiceProvider::wfpDeregister(CCommand* cmd)
{
    return WFS_ERR_INTERNAL_ERROR;
}

HRESULT CServiceProvider::wfpExecute(CCommand* cmd)
{
    return WFS_ERR_INTERNAL_ERROR;
}

HRESULT CServiceProvider::wfpGetInfo(CCommand* cmd)
{
    return WFS_ERR_INTERNAL_ERROR;
}

HRESULT CServiceProvider::wfpLock(CCommand* cmd)
{
    return WFS_ERR_INTERNAL_ERROR;
}

HRESULT CServiceProvider::wfpUnlock(CCommand* cmd)
{
    return WFS_ERR_INTERNAL_ERROR;
}

HRESULT CServiceProvider::allocateBuffer(LPWFSRESULT* bufferPointer)
{
    HRESULT res = WFMAllocateBuffer(sizeof(WFSRESULT), WFS_MEM_SHARE | WFS_MEM_ZEROINIT, (LPVOID*)bufferPointer);
    
    if (res != WFS_SUCCESS) {
        (*bufferPointer) = NULL;
    }

    return res;
}

void CServiceProvider::setCommonData(LPWFSRESULT result, CCommand* cmd)
{
    if (result != NULL) {
        SYSTEMTIME time;
        GetSystemTime(&time);

        result->hResult = WFS_SUCCESS;
        result->hService = cmd->getService();
        result->lpBuffer = NULL;
        result->RequestID = cmd->getRedId();
        result->tsTimestamp = time;
        result->u.dwCommandCode = cmd->getCommand();
    }
}

void CServiceProvider::postMessageToWindow(CCommand* cmd)
{
    while (PostMessage(cmd->getHWND(), cmd->getSPIFunction(), 0, (LPARAM)cmd->getResult()) == FALSE) {
        Sleep(32L);
    }
}

void* CServiceProvider::run()
{
    CCommand* pCmdExecuting;

    while (!this->isToExit()) {
        
        pCmdExecuting = removeCommand();

        if (pCmdExecuting != NULL){
            CCommandExecuter* executer = new CCommandExecuter(this, pCmdExecuting);
            executer->start();
        }       


        Sleep(32L);
    }
    
    return NULL;
}

CCommandExecuter::CCommandExecuter(CServiceProvider* sp, CCommand* cmd)
{
    this->m_sp = sp;
    this->m_command = cmd;
}

/*
 Executa um comando
*/
void* CCommandExecuter::run()
{
    HRESULT hResult = WFS_ERR_INTERNAL_ERROR;

    switch (this->m_command->getSPIFunction())
    {
    case WFS_CLOSE_COMPLETE:
        hResult = m_sp->wfpClose(this->m_command);
        break;
    case WFS_OPEN_COMPLETE:
        hResult = m_sp->wfpOpen(this->m_command);
        break;
    case WFS_LOCK_COMPLETE:
        hResult = m_sp->wfpLock(this->m_command);
        break;
    case WFS_UNLOCK_COMPLETE:
        hResult = m_sp->wfpUnlock(this->m_command);
        break;
    case WFS_REGISTER_COMPLETE:
        hResult = m_sp->wfpRegister(this->m_command);
        break;
    case WFS_DEREGISTER_COMPLETE:
        hResult = m_sp->wfpDeregister(this->m_command);
        break;
    case WFS_GETINFO_COMPLETE:
        hResult = m_sp->wfpGetInfo(this->m_command);
        break;
    case WFS_EXECUTE_COMPLETE:
        hResult = m_sp->wfpExecute(this->m_command);
        break;
    default:
        break;
    }

    if (this->m_command->getResult() == NULL) {
        LPWFSRESULT res = NULL;
        
        HRESULT allocateBufferResult = this->m_sp->allocateBuffer(&res);
        
        if (allocateBufferResult == WFS_SUCCESS) {
            this->m_sp->setCommonData(res, this->m_command);
            res->hResult = hResult;
            this->m_command->setResult(res);

            this->m_sp->postMessageToWindow(this->m_command);
        }

    }    
    
    return NULL;
}
