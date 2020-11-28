// xfstest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <windows.h>
#include <iostream>
#include <string>
#include <thread>
#include "XFSAPI.H"
#include "XFSADMIN.H"
#include "XFSCONF.H"
#include <XFSSIU.H>
#include <brxutil.h>

#define CONSOLE_TAB "\t\t\t\t\t\t"

boolean bStopThread = false;
HWND messageWindow = NULL;
LPCWSTR lpszClass = L"XFSTEST";
HINSTANCE hInst = NULL;

HANDLE hModuleThread = NULL;
HANDLE hRegEvent = NULL;
HSERVICE hService;
bool OpenOK = false;

using namespace std;

boolean CreateXFSMonitor();
boolean RegisterCallback();
LRESULT CALLBACK  PostCallBack(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
DWORD FAR PASCAL ThreadMonitor(string name);
void ExecuteCommands();
void ExecuteGetInfo();
void ExecuteCloseAndClening();
void PrintSiuStatus(LPWFSSIUSTATUS stats);
void PrintSiuPortEvent(LPWFSSIUPORTEVENT event);
void PrintSiuCaps(LPWFSSIUCAPS caps);

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
        OpenOK = true;

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

    if (OpenOK) {
        ExecuteCommands();
        ExecuteGetInfo();
        ExecuteCloseAndClening();
    }
}

void ExecuteCommands() {
    cout << "Press ENTER>>>>>>>>" << endl;
    cin.ignore();
    cout << "Executing SIU Commands..." << endl;
    
    REQUESTID requestID;
    HRESULT hResult;

    //Resets
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

    //Power Save
    WFSSIUPOWERSAVECONTROL PowerSaveControl;
    memset(&PowerSaveControl, 0x00, sizeof(WFSSIUPOWERSAVECONTROL));
    PowerSaveControl.usMaxPowerSaveRecoveryTime = 10;
    hResult = WFSAsyncExecute(hService, WFS_CMD_SIU_POWER_SAVE_CONTROL, &PowerSaveControl, (30 * 1000), messageWindow, &requestID);
    cout << "Exec(power_save)->ReqID(" << dec << requestID << ") = " << hResult << endl;

    //Set(Door)
    WFSSIUSETDOOR SetDoor;
    memset(&SetDoor, 0x00, sizeof(WFSSIUSETDOOR));
    SetDoor.wDoor       = WFS_SIU_SAFE;
    SetDoor.fwCommand   = WFS_SIU_UNBOLT;
    hResult = WFSAsyncExecute(hService, WFS_CMD_SIU_SET_DOOR, &SetDoor, (30 * 1000), messageWindow, &requestID);
    cout << "Exec(set_door)->ReqID(" << dec << requestID << ") = " << hResult << endl;

    //EnableEvents
    WFSSIUENABLE Enable;
    memset(&Enable, 0x00, sizeof(WFSSIUENABLE));
    Enable.fwDoors[WFS_SIU_SAFE]        = WFS_SIU_ENABLE_EVENT;
    Enable.fwSensors[WFS_SIU_TAMPER]    = WFS_SIU_ENABLE_EVENT;
    hResult = WFSAsyncExecute(hService, WFS_CMD_SIU_ENABLE_EVENTS, &Enable, (30 * 1000), messageWindow, &requestID);
    cout << "Exec(enable)->ReqID(" << dec << requestID << ") = " << hResult << endl;
}

void ExecuteGetInfo()
{
    cout << "Press ENTER>>>>>>>>" << endl;
    cin.ignore();
    cout << "Executing GetInfo..." << endl;

    REQUESTID requestID;
    HRESULT hResult;

    hResult = WFSAsyncGetInfo(hService, WFS_INF_SIU_STATUS, NULL, (30 * 1000), messageWindow, &requestID);
    cout << "Info(status)->ReqID(" << dec << requestID << ") = " << hResult << endl;

    hResult = WFSAsyncGetInfo(hService, WFS_INF_SIU_CAPABILITIES, NULL, (30 * 1000), messageWindow, &requestID);
    cout << "Info(caps)->ReqID(" << dec << requestID << ") = " << hResult << endl;
}

void ExecuteCloseAndClening()
{
    cout << "Press ENTER>>>>>>>>" << endl;
    cin.ignore();
    cout << "Executing Close and Cleaning..." << endl;

    HRESULT hResult;

    hResult = WFSClose(hService);
    cout << "WFSClose Result: " << dec << hResult << endl;

    hResult = WFSCleanUp();
    cout << "WFSCleanUp Result: " << dec << hResult << endl;

    if (messageWindow)
        DestroyWindow(messageWindow);

    if (hInst)
        UnregisterClass(lpszClass, hInst);
}

void PrintSiuStatus(LPWFSSIUSTATUS status)
{
    if (status == NULL)
        return;

    char buf[1024];
    string aux;

    sprintf_s(buf, "%s WFSSIUSTATUS:", CONSOLE_TAB);
    cout << string(buf) << endl;
    sprintf_s(buf, "%s dwDevice: %d", CONSOLE_TAB, status->fwDevice);
    cout << string(buf) << endl;

    //Sensors
    aux.clear();
    for (int i = 0; i <= WFS_SIU_SENSORS_MAX; i++) {
        int st = (int)status->fwSensors[i] - 48;
        aux.append(to_string(st));
    }
    sprintf_s(buf, "%s fwSensors: %s", CONSOLE_TAB, aux.c_str());
    cout << string(buf) << endl;

    //Doors
    aux.clear();
    for (int i = 0; i <= WFS_SIU_DOORS_MAX; i++) {
        int st = (int)status->fwDoors[i] - 48;
        aux.append(to_string(st));
    }
    sprintf_s(buf, "%s fwDoors: %s", CONSOLE_TAB, aux.c_str());
    cout << string(buf) << endl;

    //Auxiliaries
    aux.clear();
    for (int i = 0; i <= WFS_SIU_AUXILIARIES_MAX; i++) {
        int st = (int)status->fwAuxiliaries[i] - 48;
        aux.append(to_string(st));
    }
    sprintf_s(buf, "%s fwAuxiliaries: %s", CONSOLE_TAB, aux.c_str());
    cout << string(buf) << endl;

    //GuidLights
    aux.clear();
    for (int i = 0; i <= WFS_SIU_GUIDLIGHTS_MAX; i++) {
        int st = (int)status->fwGuidLights[i] - 48;
        aux.append(to_string(st));
    }
    sprintf_s(buf, "%s fwGuidLights: %s", CONSOLE_TAB, aux.c_str());
    cout << string(buf) << endl;

    //Indicators
    aux.clear();
    for (int i = 0; i <= WFS_SIU_INDICATORS_MAX; i++) {
        int st = (int)status->fwIndicators[i] - 48;
        aux.append(to_string(st));
    }
    sprintf_s(buf, "%s fwIndicators: %s", CONSOLE_TAB, aux.c_str());
    cout << string(buf) << endl;
}

void PrintSiuPortEvent(LPWFSSIUPORTEVENT event)
{
    if (event == NULL)
        return;

    char buf[1024];
    string aux;

    sprintf_s(buf, "%s WFSSIUPORTEVENT:", CONSOLE_TAB);
    cout << string(buf) << endl;
    sprintf_s(buf, "%s wPortType: %d, wPortIndex: %d, wPortStatus: %d", CONSOLE_TAB, event->wPortType, event->wPortIndex, event->wPortStatus);
    cout << string(buf) << endl;
}

void PrintSiuCaps(LPWFSSIUCAPS caps)
{
    if (caps == NULL)
        return;

    char buf[1024];
    string aux;

    sprintf_s(buf, "%s WFSSIUCAPS:", CONSOLE_TAB);
    cout << string(buf) << endl;
    sprintf_s(buf, "%s fwType: %d", CONSOLE_TAB, caps->fwType);
    cout << string(buf) << endl;

    //Sensors
    aux.clear();
    for (int i = 0; i <= WFS_SIU_SENSORS_MAX; i++) {
        int st = (int)caps->fwSensors[i] - 48;
        aux.append(to_string(st));
    }
    sprintf_s(buf, "%s fwSensors: %s", CONSOLE_TAB, aux.c_str());
    cout << string(buf) << endl;

    //Doors
    aux.clear();
    for (int i = 0; i <= WFS_SIU_DOORS_MAX; i++) {
        int st = (int)caps->fwDoors[i] - 48;
        aux.append(to_string(st));
    }
    sprintf_s(buf, "%s fwDoors: %s", CONSOLE_TAB, aux.c_str());
    cout << string(buf) << endl;

    //Auxiliaries
    aux.clear();
    for (int i = 0; i <= WFS_SIU_AUXILIARIES_MAX; i++) {
        int st = (int)caps->fwAuxiliaries[i] - 48;
        aux.append(to_string(st));
    }
    sprintf_s(buf, "%s fwAuxiliaries: %s", CONSOLE_TAB, aux.c_str());
    cout << string(buf) << endl;

    //GuidLights
    aux.clear();
    for (int i = 0; i <= WFS_SIU_GUIDLIGHTS_MAX; i++) {
        int st = (int)caps->fwGuidLights[i] - 48;
        aux.append(to_string(st));
    }
    sprintf_s(buf, "%s fwGuidLights: %s", CONSOLE_TAB, aux.c_str());
    cout << string(buf) << endl;

    //Indicators
    aux.clear();
    for (int i = 0; i <= WFS_SIU_INDICATORS_MAX; i++) {
        int st = (int)caps->fwIndicators[i] - 48;
        aux.append(to_string(st));
    }
    sprintf_s(buf, "%s fwIndicators: %s", CONSOLE_TAB, aux.c_str());
    cout << string(buf) << endl;
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
    {
        LPWFSRESULT res = (LPWFSRESULT)lParam;

        if (res != NULL) {
            char buf[1024];
            sprintf_s(buf, "%s WFS_SERVICE_EVENT(ReqID=%d)->hResult: %d", CONSOLE_TAB, res->RequestID, res->hResult);
            cout << string(buf) << endl;

            switch (res->u.dwEventID) {
            case WFS_SRVE_SIU_PORT_STATUS:
                PrintSiuPortEvent((LPWFSSIUPORTEVENT)res->lpBuffer);
                break;
            }

            WFSFreeResult(res);
        }
        else {
            cout << "WFS_SERVICE_EVENT" << endl;
        }
    }
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
    {
        LPWFSRESULT res = (LPWFSRESULT)lParam;

        if (res != NULL) {
            char buf[1024];
            sprintf_s(buf, "%s WFS_GETINFO_COMPLETE(ReqID=%d)->hResult: %d", CONSOLE_TAB, res->RequestID, res->hResult);
            cout << string(buf) << endl;

            switch (res->u.dwCommandCode) {
            case WFS_INF_SIU_STATUS:
                if (res->lpBuffer != NULL) {
                    PrintSiuStatus((LPWFSSIUSTATUS)res->lpBuffer);
                }
                break;
            case WFS_INF_SIU_CAPABILITIES:
                if (res->lpBuffer != NULL) {
                    PrintSiuCaps((LPWFSSIUCAPS)res->lpBuffer);
                }
                break;
            }

            WFSFreeResult(res);
        }
        else {
            cout << "WFS_GETINFO_COMPLETE" << endl;
        }
    }
        return 0;
    case WFS_EXECUTE_COMPLETE:
    {
        LPWFSRESULT res = (LPWFSRESULT)lParam;

        if (res != NULL) {
            char buf[1024];
            sprintf_s(buf, "%s WFS_EXECUTE_COMPLETE(ReqID=%d)->hResult: %d", CONSOLE_TAB, res->RequestID, res->hResult);            
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