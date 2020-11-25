// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include<XFSSPI.H>
#include<XFSSIU.H>
#include<brxutil.h>
#include<vector>
#include "CCommand.h"
#include "CServiceProvider.h"
#include "CObjectContainer.h"

using namespace std;

bool initializeSP() {
    bool ret = false;

    try {
        CServiceProvider* sp = NULL;
        sp = CObjectContainer::getSP();
        sp->start();

        while (!sp->isRunning()) {
            Sleep(32L);
        }

        ret = true;
    }
    catch (...) {
        TRACE("Error initializeSP");
    }

    return ret;    
}

bool finalizeSP() {
    bool ret = false;

    try {
        CServiceProvider* sp = CObjectContainer::getSP();
        sp->stop();
        sp->join();
        CObjectContainer::clearSP();

        ret = true;
    }
    catch (...) {
        TRACE("Error finalizeSP");
    }
    
    return ret;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

HRESULT extern WINAPI WFPCancelAsyncRequest(HSERVICE hService, REQUESTID RequestID) {
    TRACE("Entrei na funcao WFPCancelAsyncRequest ...");

    return WFS_ERR_INTERNAL_ERROR;
}

HRESULT extern WINAPI WFPClose(HSERVICE hService, HWND hWnd, REQUESTID ReqID) {
    TRACE("Entrei na funcao WFPClose ...");
    
    TRACE("hService: %d", hService);
    TRACE("hWnd: %02X", hWnd);
    TRACE("ReqID: %d", ReqID);
    
    HRESULT hResult = WFS_ERR_INTERNAL_ERROR;

    CSession* session = CObjectContainer::findSession(hService);

    if (session != NULL) {
        CServiceProvider* sp = CObjectContainer::getSP();
        hResult = sp->insertCommand(new CCommand(ReqID, hService, hWnd, WFS_CLOSE_COMPLETE, 0, 0, NULL, 0, NULL, 0, session));

        //TODO: aguardar terminar de cancelar os comandos agendados
        //while (sp->getWaitExecutionState() != WS_COMPLETE) {
        //    Sleep(32L);
        //}

        //TODO: terminar threads
        //if (!finalizeSP()) {
        //    return hResult;
        //}
    }

    TRACE("WFPClose Finalizado. hResult: %d", hResult);
    return hResult;
}

HRESULT extern WINAPI WFPDeregister(HSERVICE hService, DWORD dwEventClass, HWND hWndReg, HWND hWnd, REQUESTID ReqID) {
    TRACE("Entrei na funcao WFPDeregister ...");

    return WFS_ERR_INTERNAL_ERROR;
}

HRESULT extern WINAPI WFPExecute(HSERVICE hService, DWORD dwCommand, LPVOID lpCmdData, DWORD dwTimeOut, HWND hWnd, REQUESTID ReqID) {
    TRACE("Entrei na funcao WFPExecute ...");
    TRACE("hService: %d", hService);
    TRACE("dwCommand: %d", dwCommand);
   // TRACE("lpCmdData: %b", (lpCmdData != NULL));//TODO verificar uma forma de logar somente para saber se é nulo ou não o lpCmdData
    TRACE("dwTimeOut: %d", dwTimeOut);
    TRACE("hWnd: %02X", hWnd);
    TRACE("ReqID: %d", ReqID);

    HRESULT hResult = WFS_ERR_INTERNAL_ERROR;

    CServiceProvider* sp = CObjectContainer::getSP();
    CSession* session = CObjectContainer::findSession(hService);

    if (sp != NULL && session != NULL) {
        hResult = sp->insertCommand(new CCommand(ReqID, hService, hWnd, WFS_EXECUTE_COMPLETE, dwTimeOut, 0, NULL, dwCommand, lpCmdData, 0, session));
    }

    TRACE("WFPExecute Finalizado. hResult: %d", hResult);
    return hResult;
}

HRESULT extern WINAPI WFPGetInfo(HSERVICE hService, DWORD dwCategory, LPVOID lpQueryDetails, DWORD dwTimeOut, HWND hWnd, REQUESTID ReqID) {
    TRACE("Entrei na funcao WFPGetInfo");

    return WFS_ERR_INTERNAL_ERROR;
}

HRESULT extern WINAPI WFPLock(HSERVICE hService, DWORD dwTimeOut, HWND hWnd, REQUESTID ReqID) {
    TRACE("Entrei na funcao WFPLock ...");
    

    return WFS_ERR_INTERNAL_ERROR;
}

HRESULT extern WINAPI WFPOpen(HSERVICE hService, LPSTR lpszLogicalName, HAPP hApp, LPSTR lpszAppID, DWORD dwTraceLevel, DWORD dwTimeOut, HWND hWnd, REQUESTID ReqID, HPROVIDER hProvider, DWORD dwSPIVersionsRequired, LPWFSVERSION lpSPIVersion, DWORD dwSrvcVersionsRequired, LPWFSVERSION lpSrvcVersion) {
    HRESULT hResult = WFS_ERR_INTERNAL_ERROR;
    TRACE("Executando o WFPOpen ...");

    TRACE("lpszLogicalName (SIU): %s", lpszLogicalName);
    TRACE("hApp: %02X", hApp);
    TRACE("lpszAppID: %s", lpszAppID);
    TRACE("dwTraceLevel: %d", dwTraceLevel);
    TRACE("dwTimeOut: %d", dwTimeOut);
    TRACE("hWnd: %02X", hWnd);
    TRACE("ReqID: %d", ReqID);
    TRACE("hProvider: %02X", hProvider);
    TRACE("dwSPIVersionsRequired: %02X", dwSPIVersionsRequired);
    TRACE("dwSPIVersionsRequired.MaiorVersao: %02X", MAIORVERSAO(dwSPIVersionsRequired));
    TRACE("dwSPIVersionsRequired.MenorVersao: %02X", MENORVERSAO(dwSPIVersionsRequired));
    TRACE("dwSrvcVersionsRequired.MaiorVersao: %02X", MAIORVERSAO(dwSrvcVersionsRequired));
    TRACE("dwSrvcVersionsRequired.MenorVersao: %02X", MENORVERSAO(dwSrvcVersionsRequired));
    
    //Setando informacoes
    WFMSetTraceLevel(hService, dwTraceLevel);

    lpSrvcVersion->wVersion     = 0x0A03;
    lpSrvcVersion->wLowVersion  = 0x0003;
    lpSrvcVersion->wHighVersion = 0x0A03;

    memset(lpSrvcVersion->szDescription, 0x00, WFSDDESCRIPTION_LEN + 1);
    strcpy_s(lpSrvcVersion->szDescription, "Meu primeiro SP");

    memset(lpSrvcVersion->szSystemStatus, 0x00, WFSDSYSSTATUS_LEN + 1);
    strcpy_s(lpSrvcVersion->szSystemStatus, "ainda em desenvolvimento");

    if (lpSPIVersion != NULL) {
        lpSPIVersion->wVersion = lpSrvcVersion->wVersion;
        lpSPIVersion->wLowVersion = lpSrvcVersion->wLowVersion;
        lpSPIVersion->wHighVersion = lpSrvcVersion->wHighVersion;

        memset(lpSPIVersion->szDescription, 0x00, WFSDDESCRIPTION_LEN + 1);
        strcpy_s(lpSPIVersion->szDescription, lpSrvcVersion->szDescription);

        memset(lpSPIVersion->szSystemStatus, 0x00, WFSDSYSSTATUS_LEN + 1);
        strcpy_s(lpSPIVersion->szSystemStatus, lpSrvcVersion->szSystemStatus);
    }

    //

    if (!initializeSP())
        return hResult;

    CServiceProvider* sp = CObjectContainer::getSP();    
    sp->setLogicalName(lpszLogicalName);

    hResult = sp->insertCommand(new CCommand(ReqID, hService, hWnd, WFS_OPEN_COMPLETE, dwTimeOut, 0, NULL, 0));

    TRACE("WFPOpen Finalizado. hResult: %d", hResult);
    return hResult;
}

HRESULT extern WINAPI WFPRegister(HSERVICE hService, DWORD dwEventClass, HWND hWndReg, HWND hWnd, REQUESTID ReqID) {
    HRESULT hResult = WFS_ERR_INTERNAL_ERROR;
    TRACE("Executando o WFPRegister ...");

    TRACE("hService: %d", hService);
    TRACE("dwEventClass: %02X", dwEventClass);
    TRACE("hWndReg: %02X", hWndReg);
    TRACE("hWnd: %02X", hWnd);
    TRACE("ReqID: %d", ReqID);

    //
    CServiceProvider* sp = CObjectContainer::getSP();
    CSession* session = CObjectContainer::findSession(hService);

    if (sp != NULL && session != NULL) {
        hResult = sp->insertCommand(new CCommand(ReqID, hService, hWnd, WFS_REGISTER_COMPLETE, 0, dwEventClass, hWndReg, 0, NULL, 0, session));       
    }

    TRACE("WFPRegister Finalizado. hResult: %d", hResult);
    return hResult;
}

HRESULT extern WINAPI WFPSetTraceLevel(HSERVICE hService, DWORD dwTraceLevel) {
    TRACE("Entrei na funcao WFPSetTraceLevel ...");

    return WFS_ERR_INTERNAL_ERROR;
}

HRESULT extern WINAPI WFPUnloadService() {
    TRACE("Entrei na funcao WFPUnloadService ...");

    return WFS_ERR_INTERNAL_ERROR;
}

HRESULT extern WINAPI WFPUnlock(HSERVICE hService, HWND hWnd, REQUESTID) {
    TRACE("Entrei na funcao WFPUnlock ...");

    return WFS_ERR_INTERNAL_ERROR;
}


