// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include<XFSSPI.H>
#include<XFSSIU.H>

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
    WFMOutputTraceData(const_cast<LPSTR>("Entrei na funcao WFPCancelAsyncRequest ..."));

    return WFS_ERR_INTERNAL_ERROR;
}

HRESULT extern WINAPI WFPClose(HSERVICE hService, HWND hWnd, REQUESTID ReqID) {
    WFMOutputTraceData(const_cast<LPSTR>("Entrei na funcao WFPClose ..."));

    return WFS_ERR_INTERNAL_ERROR;
}

HRESULT extern WINAPI WFPDeregister(HSERVICE hService, DWORD dwEventClass, HWND hWndReg, HWND hWnd, REQUESTID ReqID) {
    WFMOutputTraceData(const_cast<LPSTR>("Entrei na funcao WFPDeregister ..."));

    return WFS_ERR_INTERNAL_ERROR;
}

HRESULT extern WINAPI WFPExecute(HSERVICE hService, DWORD dwCommand, LPVOID lpCmdData, DWORD dwTimeOut, HWND hWnd, REQUESTID ReqID) {
    WFMOutputTraceData(const_cast<LPSTR>("Entrei na funcao WFPExecute ..."));

    return WFS_ERR_INTERNAL_ERROR;
}

HRESULT extern WINAPI WFPGetInfo(HSERVICE hService, DWORD dwCategory, LPVOID lpQueryDetails, DWORD dwTimeOut, HWND hWnd, REQUESTID ReqID) {
    WFMOutputTraceData(const_cast<LPSTR>("Entrei na funcao WFPGetInfo ..."));

    return WFS_ERR_INTERNAL_ERROR;
}

HRESULT extern WINAPI WFPLock(HSERVICE hService, DWORD dwTimeOut, HWND hWnd, REQUESTID ReqID) {
    WFMOutputTraceData(const_cast<LPSTR>("Entrei na funcao WFPLock ..."));

    return WFS_ERR_INTERNAL_ERROR;
}

HRESULT extern WINAPI WFPOpen(HSERVICE hService, LPSTR lpszLogicalName, HAPP hApp, LPSTR lpszAppID, DWORD dwTraceLevel, DWORD dwTimeOut, HWND hWnd, REQUESTID ReqID, HPROVIDER hProvider, DWORD dwSPIVersionsRequired, LPWFSVERSION lpSPIVersion, DWORD dwSrvcVersionsRequired, LPWFSVERSION lpSrvcVersion) {
    WFMOutputTraceData(const_cast<LPSTR>("Entrei na funcao WFPOpen ..."));

    return WFS_ERR_INTERNAL_ERROR;
}

HRESULT extern WINAPI WFPRegister(HSERVICE hService, DWORD dwEventClass, HWND hWndReg, HWND hWnd, REQUESTID ReqID) {
    WFMOutputTraceData(const_cast<LPSTR>("Entrei na funcao WFPRegister ..."));

    return WFS_ERR_INTERNAL_ERROR;
}

HRESULT extern WINAPI WFPSetTraceLevel(HSERVICE hService, DWORD dwTraceLevel) {
    WFMOutputTraceData(const_cast<LPSTR>("Entrei na funcao WFPSetTraceLevel ..."));

    return WFS_ERR_INTERNAL_ERROR;
}

HRESULT extern WINAPI WFPUnloadService() {
    WFMOutputTraceData(const_cast<LPSTR>("Entrei na funcao WFPUnloadService ..."));

    return WFS_ERR_INTERNAL_ERROR;
}

HRESULT extern WINAPI WFPUnlock(HSERVICE hService, HWND hWnd, REQUESTID) {
    WFMOutputTraceData(const_cast<LPSTR>("Entrei na funcao WFPUnlock ..."));

    return WFS_ERR_INTERNAL_ERROR;
}


