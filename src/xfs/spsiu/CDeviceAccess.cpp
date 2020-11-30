#include "CDeviceAccess.h"
#include <XFSSIU.H>
#include "CObjectContainer.h"

HRESULT CDeviceAccess::DoDeviceOpen(CCommand* cmd)
{
    HRESULT hResult = WFS_ERR_HARDWARE_ERROR;

    DWORD deviceResult = SisOpen(const_cast <LPSTR>("Simulador"));

    if (deviceResult != SISEMU_OK)
        return hResult;

    
    //Pede status
    SISEMUSTATUS status;
    deviceResult = SisStatus(&status);

    if (deviceResult != SISEMU_OK) {
        SisClose();
        return hResult;
    }

    map_siu_status(status);

    //Pede configuração do dispositivo
    SISEMUCAP caps;
    deviceResult = SisCap(&caps);

    if (deviceResult != SISEMU_OK) {
        SisClose();
        return hResult;
    }

    map_siu_caps(caps);

    //Instancia e inicia a thread de monitoração dos status
    CStatusChecker* statuschecker = new CStatusChecker(this);
    statuschecker->start();
    m_statuschecker = statuschecker;

    hResult = WFS_SUCCESS;

    return hResult;
}

void CDeviceAccess::map_siu_status(SISEMUSTATUS status) {
    memset(&m_siu_status, 0x00, sizeof(WFSSIUSTATUS));

    //Sensors
    for (int i = 0; i <= WFS_SIU_SENSORS_MAX; i++) {
        m_siu_status.fwSensors[i] = status.Sensors[i];
    }

    //Doors
    for (int i = 0; i <= WFS_SIU_DOORS_MAX; i++) {
        m_siu_status.fwDoors[i] = status.Doors[i];
    }
}

void CDeviceAccess::map_siu_caps(SISEMUCAP cap) {
    memset(&m_siu_caps, 0x00, sizeof(WFSSIUCAPS));

    //Sensors
    for (int i = 0; i <= WFS_SIU_SENSORS_MAX; i++) {
        m_siu_caps.fwSensors[i] = cap.Sensors[i];
    }

    //Doors
    for (int i = 0; i <= WFS_SIU_DOORS_MAX;i++) {
        m_siu_caps.fwDoors[i] = cap.Doors[i];
    }

    // e fazer o mesmo para os demais ports ....
}

HRESULT CDeviceAccess::DoDeviceClose(CCommand* cmd)
{
    HRESULT hResult = WFS_ERR_HARDWARE_ERROR;

    //párar a thread de monitoração de status
    ((CStatusChecker*)m_statuschecker)->stop();
    ((CStatusChecker*)m_statuschecker)->join();

    DWORD deviceResult = SisClose();

    if (deviceResult == SISEMU_OK)
        hResult = WFS_SUCCESS;

    return hResult;
}

HRESULT CDeviceAccess::DoDeviceReset(CCommand* cmd)
{
    HRESULT hResult = WFS_ERR_HARDWARE_ERROR;

    DWORD timeout = GetTickCount() + cmd->getTimeout();
    
    DWORD deviceResult = SisReset();

    if (deviceResult == SISEMU_OK) {
        if (GetTickCount() > timeout)
            hResult = WFS_ERR_TIMEOUT;
        else
            hResult = WFS_SUCCESS;
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

    //Sensors
    for (int i = 0; i <= WFS_SIU_SENSORS_MAX; i++) {
        if (lpEnable->fwSensors[i] != WFS_SIU_NO_CHANGE) {
            if ((m_siu_caps.fwSensors[i] >= 0) && ((lpEnable->fwSensors[i] >= 0) && (lpEnable->fwSensors[i] <= WFS_SIU_SENSORS_MAX))) {
                char status = 0;
                if (lpEnable->fwSensors[i] == WFS_SIU_ENABLE_EVENT) {
                    status = ENABLE;
                }
                else {
                    status = DISABLE;
                }

                ((CStatusChecker*)m_statuschecker)->setSensor(lpEnable->fwSensors[i], status);
            }
        }
    }

    //Doors
    for (int i = 0; i <= WFS_SIU_DOORS_MAX; i++) {
        if (lpEnable->fwDoors[i] != WFS_SIU_NO_CHANGE) {
            if ((m_siu_caps.fwDoors[i] >= 0) && ((lpEnable->fwDoors[i] >= 0) && (lpEnable->fwDoors[i] <= WFS_SIU_DOORS_MAX))) {
                char status = 0;
                if (lpEnable->fwDoors[i] == WFS_SIU_ENABLE_EVENT) {
                    status = ENABLE;
                }
                else {
                    status = DISABLE;
                }

                ((CStatusChecker*)m_statuschecker)->setDoor(lpEnable->fwDoors[i], status);
            }
        }
    }

    //Fazer igual para os demais ports...

    hResult = WFS_SUCCESS;

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
    
    //Processar o input
    if (lpSetDoor->wDoor >= 0 && lpSetDoor->wDoor <= WFS_SIU_DOORS_MAX) {
        m_siu_status.fwDoors[lpSetDoor->wDoor] = lpSetDoor->fwCommand;
        hResult = WFS_SUCCESS;
    }
    else {
        hResult = WFS_ERR_SIU_SYNTAX;
    }

    return hResult;
}

HRESULT CDeviceAccess::DoDeviceSetIndicator(CCommand* cmd)
{
    HRESULT hResult = WFS_ERR_HARDWARE_ERROR;

    LPVOID buffer = cmd->getBuffer();

    if (buffer == NULL) {
        hResult = WFS_ERR_INVALID_POINTER;
        return hResult;
    }

    LPWFSSIUSETINDICATOR lpSetIndicator = (LPWFSSIUSETINDICATOR)buffer;

    //Processar o input
    if (lpSetIndicator->wIndicator >= 0 && lpSetIndicator->wIndicator <= WFS_SIU_INDICATORS_MAX) {
        m_siu_status.fwIndicators[lpSetIndicator->wIndicator] = lpSetIndicator->fwCommand;
        hResult = WFS_SUCCESS;
    }
    else {
        hResult = WFS_ERR_SIU_SYNTAX;
    }

    return hResult;
}

HRESULT CDeviceAccess::DoDeviceSetAuxiliary(CCommand* cmd)
{
    HRESULT hResult = WFS_ERR_HARDWARE_ERROR;

    LPVOID buffer = cmd->getBuffer();

    if (buffer == NULL) {
        hResult = WFS_ERR_INVALID_POINTER;
        return hResult;
    }

    LPWFSSIUSETAUXILIARY lpSetAuxiliary = (LPWFSSIUSETAUXILIARY)buffer;

    //Processar o input
    if (lpSetAuxiliary->wAuxiliary >= 0 && lpSetAuxiliary->wAuxiliary <= WFS_SIU_AUXILIARIES_MAX) {
        m_siu_status.fwAuxiliaries[lpSetAuxiliary->wAuxiliary] = lpSetAuxiliary->fwCommand;
        hResult = WFS_SUCCESS;
    }
    else {
        hResult = WFS_ERR_SIU_SYNTAX;
    }

    return hResult;
}

HRESULT CDeviceAccess::DoDeviceSetGuidLight(CCommand* cmd)
{
    HRESULT hResult = WFS_ERR_HARDWARE_ERROR;

    LPVOID buffer = cmd->getBuffer();

    if (buffer == NULL) {
        hResult = WFS_ERR_INVALID_POINTER;
        return hResult;
    }

    LPWFSSIUSETGUIDLIGHT lpSetGuidLight = (LPWFSSIUSETGUIDLIGHT)buffer;;

    //Processar o input
    if (lpSetGuidLight->wGuidLight >= 0 && lpSetGuidLight->wGuidLight <= WFS_SIU_GUIDLIGHTS_MAX) {
        m_siu_status.fwGuidLights[lpSetGuidLight->wGuidLight] = lpSetGuidLight->fwCommand;
        hResult = WFS_SUCCESS;
    }
    else {
        hResult = WFS_ERR_SIU_SYNTAX;
    }

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

CStatusChecker::CStatusChecker(CDeviceAccess* device)
{
    m_device = device;
}

void CStatusChecker::notifyStatusChange(short type, short sensor, short status)
{
    CServiceProvider* sp = CObjectContainer::getSP();

    WFSSIUPORTEVENT event;
    memset(&event, 0x00, sizeof(WFSSIUPORTEVENT));

    event.wPortType = type;
    event.wPortIndex = sensor;
    event.wPortStatus = status;
    event.lpszExtra = NULL;
    
    sp->notifyEvent(WFS_SUCCESS, WFS_SERVICE_EVENT, WFS_SRVE_SIU_PORT_STATUS, &event, sizeof(WFSSIUPORTEVENT));
}

void* CStatusChecker::run()
{
    SISEMUSTATUS currentStatus;
    while (!this->isToExit()) {
        
        if (SisStatus(&currentStatus) == SISEMU_OK) {
            //Verificar se há diferença - Sensors
            if (memcmp(currentStatus.Sensors, m_lastStatus.Sensors, sizeof(m_lastStatus.Sensors)) != 0) {
                
                //Varrer a lista de sensores
                for (int i = 0; i < SISEMU_SENSOR_MAX; i++) {
                    //Verifica se a notificação de eventos está habilitada para aquele sensor
                    if (m_enabledSensors.Sensors[i] == ENABLE) {
                        //Verifica se há difenrença entre o status atual e o anterior
                        if (currentStatus.Sensors[i] != m_lastStatus.Sensors[i]) {
                            this->notifyStatusChange(WFS_SIU_SENSORS, i, currentStatus.Sensors[i]);
                        }
                    }
                }
            }

            if (memcmp(currentStatus.Doors, m_lastStatus.Doors, sizeof(m_lastStatus.Doors)) != 0) {

                //Varrer a lista de sensores
                for (int i = 0; i < SISEMU_DOOR_MAX; i++) {
                    //Verifica se a notificação de eventos está habilitada para aquele sensor
                    if (m_enabledSensors.Doors[i] == ENABLE) {
                        //Verifica se há difenrença entre o status atual e o anterior
                        if (currentStatus.Doors[i] != m_lastStatus.Doors[i]) {
                            this->notifyStatusChange(WFS_SIU_DOORS, i, currentStatus.Doors[i]);
                        }
                    }
                }
            }

            //e assim por diante, para as demais ports

            memcpy(&m_lastStatus, &currentStatus, sizeof(SISEMUSTATUS));
        }

        Sleep(1000 * 3);
    }

    return NULL;
}

void CStatusChecker::setSensor(short sensor, short value) {    
    if (sensor >= 0 && sensor <= WFS_SIU_SENSORS_MAX)
        m_enabledSensors.Sensors[sensor] = value;
}

void CStatusChecker::setDoor(short sensor, short value) {
    if (sensor >= 0 && sensor <= WFS_SIU_DOORS_MAX)
        m_enabledSensors.Doors[sensor] = value;
}