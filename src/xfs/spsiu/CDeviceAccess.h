#pragma once
#include <Windows.h>
#include "CCommand.h"
#include <Sisemu.h>

class CDeviceAccess
{
public:
	HRESULT DoDeviceOpen(CCommand* cmd);
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
	SISEMUCAP m_caps;
};

