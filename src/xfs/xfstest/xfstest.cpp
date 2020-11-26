// xfstest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <windows.h>
#include <iostream>
#include <thread>
#include "XFSAPI.H"
#include "XFSADMIN.H"
#include "XFSCONF.H"
#include <XFSSIU.H>
#include <brxutil.h>

boolean bStopThread = false;
HWND messageWindow = NULL;
LPCWSTR lpszClass = L"XFSTEST";
HINSTANCE hInst = NULL;

HANDLE hModuleThread = NULL;
HANDLE hRegEvent = NULL;
HSERVICE hService;

using namespace std;

boolean CreateXFSMonitor();
boolean RegisterCallback();
LRESULT CALLBACK  PostCallBack(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
DWORD FAR PASCAL ThreadMonitor(string name);
void executeReset();

boolean CreateXFSMonitor() {
    cout << "---" << "Monitor de Eventos XFS" << "---" << endl;

    DWORD IdThread;

    hModuleThread = CreateThread(
        (LPSECURITY_ATTRIBUTES)NULL,
        0L,
        (LPTHREAD_START_ROUTINE)ThreadMonitor,
        (LPVOID)NULL,
        CREATE_SUSPENDED, // Nao inicia de imediato
        &IdThread); // IdThread

    if (hModuleThread) {

        hRegEvent = CreateEvent(
            NULL,    // no security
            TRUE,    // explicit reset req
            FALSE,   // initial event reset
            NULL     // no name
        );

        if (hRegEvent) {
            ResumeThread(hModuleThread);
            WaitForSingleObject(hRegEvent, 3000l);
        }
    }
    else {
        return false;
    }

    return true;
}

boolean RegisterCallback() {
    WNDCLASS WinClass;
    HWND hwndPost = NULL;

    memset(&WinClass, 0, sizeof(WNDCLASS));
    WinClass.style = 0; // Not Global
    WinClass.lpfnWndProc = (WNDPROC)PostCallBack;
    WinClass.hInstance = hInst;
    WinClass.lpszClassName = lpszClass;

    if (RegisterClass(&WinClass)) {
        hwndPost = CreateWindow(lpszClass,  /* Class Name */
            NULL,                           /* Caption */
            0,                              /* Style */
            0, 0, 0, 0,                     /* x,y,cx,cy */
            NULL,                           /* Parent Window = Desktop */
            NULL,                           /* Menu Handle */
            hInst,                          /* Module Instance */
            NULL);                          /* Initialization data */

        if (!hwndPost) {
            cout << "falha ao criar janela" << endl;
            return false;
        }
    }
    else {
        return false;
    }

    cout << "hwndPost: " << hex << hwndPost << endl;

    messageWindow = hwndPost;

    return true;
}

DWORD FAR PASCAL ThreadMonitor(string name) {
    if (!RegisterCallback())
        return 0;

    SetEvent(hRegEvent); // Processo de regitro Efetuado

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 1; // Thread finalizada normalmente
}

int main()
{
    cout << "Hello World!\n";

    HRESULT hResult;
    WFSVERSION xfs_version;
    WFSVERSION sp_version;

    hResult = WFSStartUp(VERSAO(0X0003, 0X0003), &xfs_version);

    cout << "WFSStartUp Result: " << hResult << endl;
    cout << "Description: " << xfs_version.szDescription << endl;
    cout << "SystemStatus: " << xfs_version.szSystemStatus << endl;
    cout << "HighVersion: " << hex << xfs_version.wHighVersion << endl;
    cout << "LowVersion: " << hex << xfs_version.wLowVersion << endl;
    cout << "Version: " << xfs_version.wVersion << endl;
    cout << endl;    



    hResult = WFSOpen(
        TEXTO("Sensores"),
        WFS_DEFAULT_HAPP,
        TEXTO("BRXFSTEST"),
        0,
        5000,
        VERSAO(0X0003, 0X0303),
        &xfs_version,
        &sp_version,
        &hService
    );

    cout << "WFSOpen Result: " << dec << hResult << endl;
    cout << "hService: " << dec << hService << endl;

    cout << "XFS Version: " << endl;
    cout << "Description: " << xfs_version.szDescription << endl;
    cout << "SystemStatus: " << xfs_version.szSystemStatus << endl;
    cout << "HighVersion: " << hex << xfs_version.wHighVersion << endl;
    cout << "LowVersion: " << hex << xfs_version.wLowVersion << endl;
    cout << "Version: " << xfs_version.wVersion << endl;
    cout << endl;
    cout << "SP Version: " << endl;
    cout << "Description: " << sp_version.szDescription << endl;
    cout << "SystemStatus: " << sp_version.szSystemStatus << endl;
    cout << "HighVersion: " << hex << sp_version.wHighVersion << endl;
    cout << "LowVersion: " << hex << sp_version.wLowVersion << endl;
    cout << "Version: " << sp_version.wVersion << endl;


    if (hResult == WFS_SUCCESS) {
        CreateXFSMonitor();

        if (messageWindow != NULL) {
            HRESULT hResult = WFSRegister(hService, (SYSTEM_EVENTS | USER_EVENTS | SERVICE_EVENTS | EXECUTE_EVENTS), messageWindow);
            cout << "WFSRegister Result: " << dec << hResult << endl;

            if (hResult == WFS_SUCCESS) {
                //thread monitor(ThreadMonitor, "Loop de Mensagens");
                //monitor.detach();
            }
            else {
                cout << "Error WFSRegister Result: " << dec << hResult << endl;
            }
        }
        else {
            cout << "falha ao criar janela" << endl;
        }
    }

    cin.ignore();//Exige um ENTER

    //Teste
/*    LPWFSRESULT   wfsResPointer;
    hResult = WFSExecute(hService, WFS_CMD_SIU_RESET, NULL, (30 * 1000), &wfsResPointer);
    cout << "WFSExecute Result: " << dec << hResult << endl;
    WFSFreeResult(wfsResPointer); */   
    //<--
    executeReset();

    cin.ignore();//Exige um ENTER

    hResult = WFSClose(hService);
    cout << "WFSClose Result: " << dec << hResult << endl;
    WFSCleanUp();
    cout << "WFSCleanUp Result: " << dec << hResult << endl;

    if (messageWindow)
        DestroyWindow(messageWindow);

    if (hInst)
        UnregisterClass(lpszClass, hInst);
}

void executeReset() {
    REQUESTID requestID;
    HRESULT hResult;

    for (int i = 0; i < 5; i++) {
        if (i == 2) {
            hResult = WFSCancelAsyncRequest(hService, 0);
            cout << "Exec(cancel)->ReqID(" << dec << requestID << ") = " << hResult << endl;
        }
        else {
            hResult = WFSAsyncExecute(hService, WFS_CMD_SIU_RESET, NULL, (30 * 1000), messageWindow, &requestID);
            cout << "Exec(reset)->ReqID(" << dec << requestID << ") = " << hResult << endl;
        }
    }
}

LRESULT CALLBACK  PostCallBack(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    //cout << "uMsg: " << dec << uMsg << endl;

    switch (uMsg) {
    //case WM_TIMER:
    //    cout << "WM_TIMER" << endl;
    //    return 0;
    case WFS_TIMER_EVENT:
        cout << "WM_TIMER" << endl;
        return 0;
    case WFS_EXECUTE_EVENT:
        cout << "WFS_EXECUTE_EVENT" << endl;
        return 0;
    case WFS_SERVICE_EVENT:
        cout << "WFS_SERVICE_EVENT" << endl;
        return 0;
    case WFS_USER_EVENT:
        cout << "WFS_USER_EVENT" << endl;
        return 0;
    case WFS_SYSTEM_EVENT:
        cout << "WFS_SYSTEM_EVENT" << endl;
        return 0;
    case WFS_OPEN_COMPLETE:
        cout << "WFS_OPEN_COMPLETE" << endl;
        return 0;
    case WFS_CLOSE_COMPLETE:
        cout << "WFS_CLOSE_COMPLETE" << endl;
        return 0;
    case WFS_LOCK_COMPLETE:
        cout << "WFS_LOCK_COMPLETE" << endl;
        return 0;
    case WFS_UNLOCK_COMPLETE:
        cout << "WFS_UNLOCK_COMPLETE" << endl;
        return 0;
    case WFS_REGISTER_COMPLETE:
        cout << "WFS_REGISTER_COMPLETE" << endl;
        return 0;
    case WFS_DEREGISTER_COMPLETE:
        cout << "WFS_DEREGISTER_COMPLETE" << endl;
        return 0;
    case WFS_GETINFO_COMPLETE:
        cout << "WFS_GETINFO_COMPLETE" << endl;
        return 0;
    case WFS_EXECUTE_COMPLETE:
    {
        LPWFSRESULT res = (LPWFSRESULT)lParam;

        if (res != NULL) {
            char buf[1024];
            sprintf_s(buf, "WFS_EXECUTE_COMPLETE(ReqID=%d)->hResult: %d", res->RequestID, res->hResult);            
            cout << string(buf) << endl;
            WFSFreeResult(res);
        }
        else {
            cout << "WFS_EXECUTE_COMPLETE" << endl;
        }
        return 0;
    }
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}