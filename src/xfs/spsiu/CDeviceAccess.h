#pragma once
#include <Windows.h>
#include "CCommand.h"
#include <Sisemu.h>
#include <XFSSIU.H>
#include "CThread.h"

#define ENABLE 0x01
#define DISABLE 0x00

typedef struct _sis_enable_events {
	WORD Sensors[SISEMU_SENSOR_SIZE];
	WORD Doors[SISEMU_DOOR_SIZE];
	WORD Indicators[SISEMU_INDICATOR_SIZE];
	WORD Auxiliaries[SISEMU_AUXILIARY_SIZE];
	WORD Guidlights[SISEMU_GUIDLIGHT_SIZE];
}SISEMUENABLEEVENTS, LPSISEMUENABLEEVENTS;

class CDeviceAccess
{
public:
	HRESULT DoDeviceOpen(CCommand* cmd);
	void map_siu_status(SISEMUSTATUS status);
	void map_siu_caps(SISEMUCAP cap);
	HRESULT DoDeviceClose(CCommand* cmd);
	HRESULT DoDeviceReset(CCommand* cmd);
	HRESULT DoEnableEvents(CCommand* cmd);
	HRESULT DoDeviceSetPorts(CCommand* cmd);
	HRESULT DoDeviceSetDoor(CCommand* cmd);
	HRESULT DoDeviceSetIndicator(CCommand* cmd);
	HRESULT DoDeviceSetAuxiliary(CCommand* cmd);
	HRESULT DoDeviceSetGuidLight(CCommand* cmd);		
	HRESULT GetDeviceStatus(CCommand* cmd);
	HRESULT GetDeviceCapabilities(CCommand* cmd);

	//Funções utilitárias / suporte
	HRESULT allocateBuffer(LPWFSRESULT* bufferPointer);
	HRESULT allocateMoreBuffer(ULONG size, LPVOID parent, LPVOID* buffer);
	void setCommonData(LPWFSRESULT result, CCommand* cmd);
	void postMessageToWindow(CCommand* cmd);
	void notifyEvent(HWND Wnd, HSERVICE hService, int typeEvent, int eventID, WORD wBuffer);

private:	
	WFSSIUCAPS m_siu_caps;
	WFSSIUSTATUS m_siu_status;

	LPVOID m_statuschecker;

	friend class CStatusChecker;
};

class CStatusChecker : public CThread {
public:
	CStatusChecker(CDeviceAccess* device);
	void notifyStatusChange(short type, short sensor, short status);
	void setSensor(short sensor, short value);
	void setDoor(short sensor, short value);

protected:
	void* run();
private:
	CDeviceAccess* m_device;
	SISEMUSTATUS m_lastStatus;
	SISEMUENABLEEVENTS m_enabledSensors;
};
