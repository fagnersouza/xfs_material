// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include<XFSSPI.H>
#include<XFSSIU.H>
#include<brxutil.h>

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

    return WFS_ERR_INTERNAL_ERROR;
}

HRESULT extern WINAPI WFPDeregister(HSERVICE hService, DWORD dwEventClass, HWND hWndReg, HWND hWnd, REQUESTID ReqID) {
    TRACE("Entrei na funcao WFPDeregister ...");

    return WFS_ERR_INTERNAL_ERROR;
}

HRESULT extern WINAPI WFPExecute(HSERVICE hService, DWORD dwCommand, LPVOID lpCmdData, DWORD dwTimeOut, HWND hWnd, REQUESTID ReqID) {
    TRACE("Entrei na funcao WFPExecute ...");

    return WFS_ERR_INTERNAL_ERROR;
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

    hService = 1;

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

    Sleep(2000);//Simulado algum acesso a dispositivo

    hResult = WFS_SUCCESS;

    WFSRESULT   wfsResLocal;
    memset(&wfsResLocal, 0, sizeof(wfsResLocal));

    wfsResLocal.hResult = hResult;
    wfsResLocal.hService = hService;
    wfsResLocal.RequestID = ReqID;
    GetSystemTime((LPSYSTEMTIME)&wfsResLocal.tsTimestamp);

    LPWFSRESULT lpPostRes;

    if (WFMAllocateBuffer(sizeof(WFSRESULT), WFS_MEM_ZEROINIT | WFS_MEM_SHARE, (LPVOID*)&lpPostRes) != WFS_SUCCESS) {
        TRACE("PostResult: ERROR WFMAllocateBuffer");
        return WFS_ERR_INTERNAL_ERROR;
    }

    //memcpy(lpPostRes, &wfsResLocal, sizeof(WFSRESULT));

    PostMessage(hWnd, WFS_OPEN_COMPLETE, 0, (LPARAM)lpPostRes);//Envia mensagem para a window (hWnd) definida pelo XFS Manager

    TRACE("WFPOpen Finalizado. hResult: %d", hResult);
    return hResult;
}

HRESULT extern WINAPI WFPRegister(HSERVICE hService, DWORD dwEventClass, HWND hWndReg, HWND hWnd, REQUESTID ReqID) {
    TRACE("Entrei na funcao WFPRegister ...");

    return WFS_ERR_INTERNAL_ERROR;
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


