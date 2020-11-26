#include "CServiceProvider.h"
#include "CSession.h"
#include "CObjectContainer.h"
#include <XFSSIU.H>
#include <brxutil.h>
#include "CLockController.h"


CServiceProvider::CServiceProvider()
{
    running = false;
    memset(m_logicalName, 0x00, sizeof(m_logicalName));
    m_mutCommands = new CMutex();
    m_waitCommandState = EWaitState::WS_NOTSTARTED;
    m_numberOfCommandsExecuting = 0;
    m_waitCommmandReqId = 0;
}

HRESULT CServiceProvider::insertCommand(CCommand* cmd)
{
    HRESULT hResult = WFS_ERR_INTERNAL_ERROR;

    if (cmd != NULL) {        
        if (cmd->getSPIFunction() == WFS_ERR_CANCELED) {
            hResult = wfpCancel(cmd);
        }
        else {//Agendar execução
            //acessa m_wosaQueue dentreo da região crítica para 
            //garantir que nenhum comando será cancelado ou removido
            //da lista e iniciado durante esta operação
            m_mutCommands->request();
            m_wosaQueue.push_back(cmd);
            m_mutCommands->release();
            hResult = WFS_SUCCESS;
        }
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
    
    substractNumberOfCommandsExecuting();//diminui 1 do número de comandos em execução (pois este close está em execução e só fará descrecer o contador de comandos no seu fim)
    
    wfpCancel(cmdCancel);
    addNumberOfCommandsExecuting();//aumenta 1 o numero de comandos - devido ao wfpCancel

    delete session;

    //cancelar o registro de todas as janelas do serviçe que solicitou o close
    
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
    //if (cmd->getReqId() == 0) {
    //    while (getNumberOfCommandsExecuting() > 0) {
    //        Sleep(32L);
    //    }
    //}

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

RegisteredWindow* CServiceProvider::findRegisteredWindowsByService(HSERVICE hService)
{
    RegisteredWindow* ret = NULL;

    vector<RegisteredWindow*>::iterator it = m_registeredWindows.begin();

    while (it != m_registeredWindows.end()) {
        if ((*it)->Service == hService) {
            ret = (*it);
            break;
        }

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
    RegisteredWindow* registeredWindow = NULL;

    //Verifica se a janela já foi inserida no vetor
    registeredWindow = findRegisteredWindowsByHandle(cmd->getWndReg());

    //se igual a NULL então a jenala ainda não se encontra no vetor
    if (registeredWindow != NULL) {
        bool freeArea = false;

        //Remove as classes de eento passadas como parametro
        registeredWindow->EventClass = (registeredWindow->EventClass & (~cmd->getEventClass()));

        //Verifica se todos as classes de eventos foram removidas da configuração.
        //Neste caso, o registro deve ser removido da lista
        if (registeredWindow->EventClass == 0) {
            vector<RegisteredWindow*>::iterator it = m_registeredWindows.begin();

            while (it != m_registeredWindows.end()) {
                //Verifica se o handle da lista é o da janela do processo que acabou de executar
                if ((*it) == registeredWindow) {
                    break;
                }

                it++;
            }

            m_registeredWindows.erase(it);
            freeArea = true;
        }

        //TODO: verificar complicacoes do LOCK/UNLOCK

        if (freeArea) {
            delete registeredWindow;
        }
    }    
    
    return WFS_SUCCESS;
}

HRESULT CServiceProvider::wfpExecute(CCommand* cmd)
{
    HRESULT hResult = WFS_ERR_INTERNAL_ERROR;
    
    switch (cmd->getCommand()) {
        case WFS_CMD_SIU_ENABLE_EVENTS:
            //chamar quem possa executar este comando
            break;
        case WFS_CMD_SIU_SET_PORTS:
            //chamar quem possa executar este comando
            break;
        case WFS_CMD_SIU_SET_DOOR:
            break;
        case WFS_CMD_SIU_SET_INDICATOR:
            break;
        case WFS_CMD_SIU_SET_AUXILIARY:
            break;
        case WFS_CMD_SIU_SET_GUIDLIGHT:
            break;
        case WFS_CMD_SIU_RESET:
        {
            //Procedure de Reset
            TRACE("Executar RESET");
            //faz o trampo que precisa junto ao dispositivo
            Sleep(1000);

            LPWFSRESULT res = NULL;

            if (allocateBuffer(&res) == WFS_SUCCESS) {
                setCommonData(res, cmd);
                hResult = WFS_SUCCESS;
                res->hResult = hResult;

                cmd->setResult(res);
            }
            //
        }
            break;
        case WFS_CMD_SIU_POWER_SAVE_CONTROL:
            break;
        default:
            hResult = WFS_ERR_INVALID_COMMAND;
            break;
    }
    
    return hResult;
}

HRESULT CServiceProvider::wfpGetInfo(CCommand* cmd)
{
    HRESULT hResult = WFS_ERR_INTERNAL_ERROR;
    
    switch (cmd->getCommand()) {
        case WFS_INF_SIU_STATUS:
            //Delegar para DeviceAccess função que resolva status de dispositivo SIU
            break;
        case WFS_INF_SIU_CAPABILITIES:
            //Delegar para DeviceAccess função que resolva capabilities de dispositivo SIU
            break;
        default:
            hResult = WFS_ERR_INVALID_CATEGORY;
    }

    return hResult;
}

HRESULT CServiceProvider::wfpLock(CCommand* cmd)
{
    //Verifica se o processa já não possui a trava
    if (!CLockController::hasLock(cmd->getHService())) {
        ///////WFS_SYSE_LOCK_REQUESTED
        
        //Tenta obter a janela registrada pelo serviço
        RegisteredWindow* tmpRegisteredWindow = findRegisteredWindowsByService(cmd->getHService());

        //Obtem a janela de quem detém a trava
        HWND lockerWindowHandle = CLockController::getLockerWindowHandle(cmd->getHService());

        //Verifica as pré-condições para disparar o evento de tentativa de lock
        if ((lockerWindowHandle != NULL) && ((tmpRegisteredWindow == NULL) || (lockerWindowHandle != tmpRegisteredWindow->WndReg))) {
            notifyEvent(lockerWindowHandle, cmd->getHService(), WFS_SYSTEM_EVENT, WFS_SYSE_LOCK_REQUESTED, NULL);
        }

        //TODO: 

    }
    
    return WFS_ERR_INTERNAL_ERROR;
}

HRESULT CServiceProvider::wfpUnlock(CCommand* cmd)
{
    return WFS_ERR_INTERNAL_ERROR;
}

HRESULT CServiceProvider::wfpSetTraceLevel(CCommand* cmd)
{
    HRESULT hResult = WFS_SUCCESS;

    //Usar:
    //cmd->getTraceLevel();
    //cmd->getHService();

    //Setar nível de trace da sua solucação particular
    //Inclusive, pode ser um set em arquivo de propriedade / Windows Register ou etc...

    return hResult;
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

void CServiceProvider::notifyEvent(HWND Wnd, HSERVICE hService, int typeEvent, int eventID, WORD wBuffer)
{
    SYSTEMTIME time;
    GetSystemTime(&time);

    HRESULT res;
    LPWFSRESULT result = NULL;

    res = WFMAllocateBuffer(sizeof(WFSRESULT), WFS_MEM_SHARE | WFS_MEM_ZEROINIT, (LPVOID*)&result);

    if (res != WFS_SUCCESS) {
        return;
    }

    result->hResult     = WFS_SUCCESS;
    result->hService    = hService;
    result->RequestID   = NULL;
    result->tsTimestamp = time;
    result->u.dwEventID = eventID;

    while (PostMessage(Wnd, typeEvent, 0, (LPARAM)result) == FALSE) {
        Sleep(32L);
    }
}

void CServiceProvider::startWaitExection(int reqId)
{
    m_waitCommmandReqId = reqId;
    m_waitCommandState = EWaitState::WS_NOTSTARTED;
}

int CServiceProvider::getWaitExecutionReqId()
{
    return m_waitCommmandReqId;
}

EWaitState CServiceProvider::getWaitExecutionState()
{
    return m_waitCommandState;
}

void CServiceProvider::setWaitExecutionState(EWaitState state)
{
    m_waitCommandState = state;
}

int CServiceProvider::getNumberOfCommandsExecuting()
{
    return m_numberOfCommandsExecuting;
}

void CServiceProvider::addNumberOfCommandsExecuting()
{
    m_numberOfCommandsExecuting++;
}

void CServiceProvider::substractNumberOfCommandsExecuting()
{
    m_numberOfCommandsExecuting--;
}

//Thread Principal do SP
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
            
            //Cria thread de execução do comando
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
 Thread secundária do SP->Executa um comando
*/
void* CCommandExecuter::run()
{
    //Verifica se essa request deve ser monitorada
    if (m_sp->getWaitExecutionReqId() == m_command->getReqId()) {
        m_sp->setWaitExecutionState(EWaitState::WFS_RUNNING);
    }
    
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
    else {
        this->m_sp->postMessageToWindow(this->m_command);
    }
    
    //Atualiza quantidade de comandos em execuçao (diminui)
    m_sp->substractNumberOfCommandsExecuting();

    //Atualiza estado do comando
    if (m_sp->getWaitExecutionReqId() == m_command->getReqId()) {
        m_sp->setWaitExecutionState(EWaitState::WS_COMPLETED);
    }

    return NULL;
}

void CCommandExecuter::start()
{
    m_sp->addNumberOfCommandsExecuting();
    CThread::start();
}
