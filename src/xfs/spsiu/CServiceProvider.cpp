#include "CServiceProvider.h"
#include "CSession.h"
#include "CObjectContainer.h"

CServiceProvider::CServiceProvider()
{
    running = false;
    memset(m_logicalName, 0x00, sizeof(m_logicalName));
    m_mutCommands = new CMutex();
}

HRESULT CServiceProvider::insertCommand(CCommand* cmd)
{
    HRESULT hResult = WFS_ERR_INTERNAL_ERROR;

    if (cmd != NULL) {        
        //acessa m_wosaQueue dentreo da região crítica para 
        //garantir que nenhum comando será cancelado ou removido
        //da lista e iniciado durante esta operação
        m_mutCommands->request();
        m_wosaQueue.push_back(cmd);
        m_mutCommands->release();
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
    CSession* session = new CSession(this->getLogicalName(), cmd->getHService());

    CObjectContainer::addSession(session);
    
    return WFS_SUCCESS;
}

HRESULT CServiceProvider::wfpClose(CCommand* cmd)
{
    CSession* session = cmd->getSession();

    CCommand* cmdCancel = new CCommand(0, session->getHService(), NULL, WFS_ERR_CANCELED, 0, 0, NULL, 0, NULL, 0, session);
    wfpCancel(cmdCancel);

    delete session;
    
    return WFS_SUCCESS;
}

HRESULT CServiceProvider::wfpCancel(CCommand* cmd) {
    HRESULT hResult = WFS_SUCCESS;

    CCommand* cmdFromQueue = NULL;
    HRESULT allocateBufferResult;
    LPWFSRESULT res = NULL;

    m_mutCommands->request();

    for (unsigned int i = 0; i < m_wosaQueue.size(); i++) {
        cmdFromQueue = (CCommand*)m_wosaQueue.at(i);

        //Verifica se deve cancelar TODOS os comandos ou se deve cancelar
        // um comando específico --> cmd->getReqId() == 0 (cancela TODOS os comandos que estão sendo varridos no loop)
        if (cmd->getReqId() == 0 || cmd->getReqId() == cmdFromQueue->getReqId()) {
            allocateBufferResult = this->allocateBuffer(&res);

            if (allocateBufferResult == WFS_SUCCESS) {
                setCommonData(res, cmdFromQueue);
                res->hResult = WFS_ERR_CANCELED;
                cmdFromQueue->setResult(res);
            }
        }
    }

    m_mutCommands->release();

    //TODO: verificar se é preciso controlar quantidade de comando em execução

    return hResult;
}

RegisteredWindow* CServiceProvider::findRegisteredWindowsByHandle(HWND Wnd) {
    RegisteredWindow* ret = NULL;

    //Iterator para percorrer a lista de handles
    vector<RegisteredWindow*>::iterator it = m_registeredWindows.begin();


    //Enquanto não chega ao fim da lista ...
    while (it != m_registeredWindows.end()) {
        
        //Verifica se o handle da lista é o da janela do processo
        if ((*it)->WndReg == Wnd) {
            //Retorna a estrutura encontrada
            ret = (*it);
        }

        //Avança o item atual da lista
        it++;
    }

    return ret;
}

HRESULT CServiceProvider::wfpRegister(CCommand* cmd)
{
    RegisteredWindow* registeredWindow = NULL;

    registeredWindow = this->findRegisteredWindowsByHandle(cmd->getWndReg());

    //Se igual a NULL entaão janela ainda não se encontra na lista
    if (registeredWindow == NULL) {
        registeredWindow = new RegisteredWindow;
        registeredWindow->Service = cmd->getHService();
        registeredWindow->EventClass = cmd->getEventClass();
        registeredWindow->WndReg = cmd->getWndReg();

        this->m_registeredWindows.push_back(registeredWindow);
    }
    else {//se diferente de NULL, então atualizar o EventClass registrado
        
        //Verifica se a o EventClass não foi previamente inserido na estrutura
        if ((registeredWindow->EventClass & cmd->getEventClass()) != cmd->getEventClass()) {
            registeredWindow->EventClass = (registeredWindow->EventClass | cmd->getEventClass());
        }
    }

    //TODO: verificar tratamento especial para SYSTEM_EVENT em relação ao Lock

    return WFS_SUCCESS;
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
        result->hService = cmd->getHService();
        result->lpBuffer = NULL;
        result->RequestID = cmd->getReqId();
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
    running = true;
    CCommand* pCmdExecuting;

    while (!this->isToExit()) {
        
        this->m_mutCommands->request();

        pCmdExecuting = removeCommand();

        if (pCmdExecuting != NULL){
            LPWFSRESULT pWfsResult = (LPWFSRESULT)pCmdExecuting->getResult();

            if (pWfsResult != NULL) {
                
                //Se comando foi marcado como cancelado, nem executamos
                if (pWfsResult->hResult == WFS_ERR_CANCELED) {
                    //Mandar postMessage com o retorno
                    postMessageToWindow(pCmdExecuting);
                    delete pCmdExecuting;
                    pCmdExecuting = NULL;
                }
            }
            
            if (pCmdExecuting != NULL) {
                CCommandExecuter* executer = new CCommandExecuter(this, pCmdExecuting);
                executer->start();
            }
        }

        this->m_mutCommands->release();

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
