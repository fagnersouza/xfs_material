#include "CDeviceAccess.h"
#include <XFSSIU.H>
#include <Sisemu.h>

HRESULT CDeviceAccess::DoDeviceOpen(CCommand* cmd)
{
    HRESULT hResult = WFS_ERR_HARDWARE_ERROR;

    DWORD res = SisOpen(const_cast <LPSTR>("Simulador"));

    if (res != SISEMU_OK)
        return hResult;

    //TODO:

    hResult = WFS_SUCCESS;

    return hResult;
}

HRESULT CDeviceAccess::DoDeviceClose(CCommand* cmd)
{
    HRESULT hResult = WFS_ERR_HARDWARE_ERROR;

    hResult = WFS_SUCCESS;

    return hResult;
}

HRESULT CDeviceAccess::DoDeviceReset(CCommand* cmd)
{
    HRESULT hResult = WFS_ERR_HARDWARE_ERROR;

    //faz o trampo que precisa junto ao dispositivo
    Sleep(1000);

    LPWFSRESULT res = NULL;

    if (allocateBuffer(&res) == WFS_SUCCESS) {
        setCommonData(res, cmd);
        hResult = WFS_SUCCESS;
        res->hResult = hResult;

        cmd->setResult(res);
    }

    return hResult;
}

HRESULT CDeviceAccess::DoEnableEvents(CCommand* cmd)
{
    HRESULT hResult = WFS_ERR_HARDWARE_ERROR;

    LPVOID buffer = cmd->getBuffer();

    if (buffer == NULL) {
        hResult = WFS_ERR_INVALID_POINTER;
        return hResult;
    }

    LPWFSSIUENABLE lpEnable = (LPWFSSIUENABLE)buffer;

    int i = 0;

    return hResult;
}

HRESULT CDeviceAccess::DoDeviceSetPorts(CCommand* cmd)
{
    HRESULT hResult = WFS_ERR_HARDWARE_ERROR;

    LPVOID buffer = cmd->getBuffer();

    if (buffer == NULL) {
        hResult = WFS_ERR_INVALID_POINTER;
        return hResult;
    }

    LPWFSSIUSETPORTS lpSetPorts = (LPWFSSIUSETPORTS)buffer;


    return hResult;
}

HRESULT CDeviceAccess::DoDeviceSetDoor(CCommand* cmd)
{
    HRESULT hResult = WFS_ERR_HARDWARE_ERROR;

    LPVOID buffer = cmd->getBuffer();

    if (buffer == NULL) {
        hResult = WFS_ERR_INVALID_POINTER;
        return hResult;
    }

    LPWFSSIUSETDOOR lpSetDoor = (LPWFSSIUSETDOOR)buffer;

    return hResult;
}

HRESULT CDeviceAccess::DoDeviceSetIndicator(CCommand* cmd)
{
    HRESULT hResult = WFS_ERR_HARDWARE_ERROR;

    return hResult;
}

HRESULT CDeviceAccess::DoDeviceSetAuxiliary(CCommand* cmd)
{
    HRESULT hResult = WFS_ERR_HARDWARE_ERROR;

    return hResult;
}

HRESULT CDeviceAccess::DoDeviceSetGuidLight(CCommand* cmd)
{
    HRESULT hResult = WFS_ERR_HARDWARE_ERROR;

    return hResult;
}

HRESULT CDeviceAccess::GetDeviceStatus(CCommand* cmd)
{
    HRESULT hResult = WFS_ERR_HARDWARE_ERROR;

    return hResult;
}

HRESULT CDeviceAccess::GetDeviceCapabilities(CCommand* cmd)
{
    HRESULT hResult = WFS_ERR_HARDWARE_ERROR;

    return hResult;
}

HRESULT CDeviceAccess::allocateBuffer(LPWFSRESULT* bufferPointer)
{
    HRESULT res = WFMAllocateBuffer(sizeof(WFSRESULT), WFS_MEM_SHARE | WFS_MEM_ZEROINIT, (LPVOID*)bufferPointer);

    if (res != WFS_SUCCESS) {
        (*bufferPointer) = NULL;
    }

    return res;
}

void CDeviceAccess::setCommonData(LPWFSRESULT result, CCommand* cmd)
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

void CDeviceAccess::postMessageToWindow(CCommand* cmd)
{
    while (PostMessage(cmd->getHWND(), cmd->getSPIFunction(), 0, (LPARAM)cmd->getResult()) == FALSE) {
        Sleep(32L);
    }
}

void CDeviceAccess::notifyEvent(HWND Wnd, HSERVICE hService, int typeEvent, int eventID, WORD wBuffer)
{
    SYSTEMTIME time;
    GetSystemTime(&time);

    HRESULT res;
    LPWFSRESULT result = NULL;

    res = WFMAllocateBuffer(sizeof(WFSRESULT), WFS_MEM_SHARE | WFS_MEM_ZEROINIT, (LPVOID*)&result);

    if (res != WFS_SUCCESS) {
        return;
    }

    result->hResult = WFS_SUCCESS;
    result->hService = hService;
    result->RequestID = NULL;
    result->tsTimestamp = time;
    result->u.dwEventID = eventID;

    while (PostMessage(Wnd, typeEvent, 0, (LPARAM)result) == FALSE) {
        Sleep(32L);
    }
}
