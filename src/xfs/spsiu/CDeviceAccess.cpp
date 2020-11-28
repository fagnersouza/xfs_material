#include "CDeviceAccess.h"
#include <XFSSIU.H>

HRESULT CDeviceAccess::DoDeviceOpen(CCommand* cmd)
{
    HRESULT hResult = WFS_ERR_HARDWARE_ERROR;

    DWORD deviceResult = SisOpen(const_cast <LPSTR>("Simulador"));

    if (deviceResult != SISEMU_OK)
        return hResult;

    
    deviceResult = SisCap(&m_caps);

    if (deviceResult != SISEMU_OK) {
        SisClose();
        return hResult;
    }

    SISEMUSTATUS status;
    deviceResult = SisStatus(&status);

    if (deviceResult != SISEMU_OK) {
        SisClose();
        return hResult;
    }

    //TODO: criar thread para resolver EnableEvents

    hResult = WFS_SUCCESS;

    return hResult;
}

HRESULT CDeviceAccess::DoDeviceClose(CCommand* cmd)
{
    HRESULT hResult = WFS_ERR_HARDWARE_ERROR;
    //TODO: párar thread

    DWORD deviceResult = SisClose();

    if (deviceResult == SISEMU_OK)
        hResult = WFS_SUCCESS;

    LPWFSRESULT res = NULL;
    HRESULT allocateBufferResult = allocateBuffer(&res);

    if (allocateBufferResult == WFS_SUCCESS) {
        setCommonData(res, cmd);
        cmd->setResult(res);
        res->hResult = hResult;
    }

    return hResult;
}

HRESULT CDeviceAccess::DoDeviceReset(CCommand* cmd)
{
    HRESULT hResult = WFS_ERR_HARDWARE_ERROR;
    
    DWORD deviceResult = SisReset();

    if (deviceResult == SISEMU_OK)
        hResult = WFS_SUCCESS;

    LPWFSRESULT res = NULL;

    if (allocateBuffer(&res) == WFS_SUCCESS) {
        setCommonData(res, cmd);
        cmd->setResult(res);
        res->hResult = hResult;        
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
    LPWFSRESULT res = NULL;

    //Aloca area de resultado e seta informacoes default
    HRESULT allocateBufferResult = this->allocateBuffer(&res);

    if (allocateBufferResult != WFS_SUCCESS)
        return hResult;

    setCommonData(res, cmd);

    //Consulta dispositivo
    SISEMUSTATUS status;
    memset(&status, 0x00, sizeof(SISEMUSTATUS));
    DWORD deviceResult = SisStatus(&status);

    if (deviceResult != SISEMU_OK) {
        cmd->setResult(res);
        res->hResult = hResult;
        
        return hResult;
    }

    //Monta área de resposta
    WFSSIUSTATUS xfsStatus;
    memset(&xfsStatus, 0x00, sizeof(WFSSIUSTATUS));

    //Sensors
    for (int i = 0; i <= WFS_SIU_SENSORS_MAX;i++) {
        xfsStatus.fwSensors[i] = status.Sensors[i];
    }

    //Doors
    for (int i = 0; i <= WFS_SIU_DOORS_MAX; i++) {
        xfsStatus.fwDoors[i] = status.Doors[i];
    }

    //Auxiliaries
    for (int i = 0; i <= WFS_SIU_AUXILIARIES_MAX; i++) {
        xfsStatus.fwAuxiliaries[i] = status.Auxiliaries[i];
    }

    //GuidLights
    for (int i = 0; i <= WFS_SIU_GUIDLIGHTS_MAX; i++) {
        xfsStatus.fwGuidLights[i] = status.Guidlights[i];
    }

    //Indicators
    for (int i = 0; i <= WFS_SIU_INDICATORS_MAX; i++) {
        xfsStatus.fwIndicators[i] = status.Indicators[i];
    }
    
    //Aloca area de dados adicionais
    allocateBufferResult = allocateMoreBuffer(sizeof(WFSSIUSTATUS), res, &res->lpBuffer);

    if (allocateBufferResult == WFS_SUCCESS) {
        memcpy(res->lpBuffer, &xfsStatus, sizeof(WFSSIUSTATUS));
        cmd->setResult(res);
        hResult = WFS_SUCCESS;
        res->hResult = hResult;
    }

    return hResult;
}

HRESULT CDeviceAccess::GetDeviceCapabilities(CCommand* cmd)
{
    HRESULT hResult = WFS_ERR_HARDWARE_ERROR;
    LPWFSRESULT res = NULL;

    //Aloca area de resultado e seta informacoes default
    HRESULT allocateBufferResult = this->allocateBuffer(&res);

    if (allocateBufferResult != WFS_SUCCESS)
        return hResult;

    setCommonData(res, cmd);

    //Consulta dispositivo
    SISEMUCAP cap;
    memset(&cap, 0x00, sizeof(SISEMUCAP));
    DWORD deviceResult = SisCap(&cap);

    if (deviceResult != SISEMU_OK) {
        cmd->setResult(res);
        res->hResult = hResult;

        return hResult;
    }

    //Monta área de resposta
    WFSSIUSTATUS xfsStatus;
    memset(&xfsStatus, 0x00, sizeof(WFSSIUSTATUS));

    //Sensors
    for (int i = 0; i <= WFS_SIU_SENSORS_MAX; i++) {
        xfsStatus.fwSensors[i] = cap.Sensors[i];
    }

    //Doors
    for (int i = 0; i <= WFS_SIU_DOORS_MAX; i++) {
        xfsStatus.fwDoors[i] = cap.Doors[i];
    }

    //Auxiliaries
    for (int i = 0; i <= WFS_SIU_AUXILIARIES_MAX; i++) {
        xfsStatus.fwAuxiliaries[i] = cap.Auxiliaries[i];
    }

    //GuidLights
    for (int i = 0; i <= WFS_SIU_GUIDLIGHTS_MAX; i++) {
        xfsStatus.fwGuidLights[i] = cap.Guidlights[i];
    }

    //Indicators
    for (int i = 0; i <= WFS_SIU_INDICATORS_MAX; i++) {
        xfsStatus.fwIndicators[i] = cap.Indicators[i];
    }

    //Aloca area de dados adicionais
    allocateBufferResult = allocateMoreBuffer(sizeof(WFSSIUSTATUS), res, &res->lpBuffer);

    if (allocateBufferResult == WFS_SUCCESS) {
        memcpy(res->lpBuffer, &xfsStatus, sizeof(WFSSIUSTATUS));
        cmd->setResult(res);
        hResult = WFS_SUCCESS;
        res->hResult = hResult;
    }

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

HRESULT CDeviceAccess::allocateMoreBuffer(ULONG size, LPVOID parent, LPVOID* buffer) {
    HRESULT res = WFMAllocateMore(size, parent, buffer);

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
