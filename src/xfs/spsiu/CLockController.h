#pragma once
#include <Windows.h>
#include <XFSSPI.H>
#include "CMutex.h"

class CLockController
{
private:
	//Armazena a ID do processo que está utilizando a DLL
	static DWORD processID;

	//Armazena o ID do serviço que executou a trava
	static HSERVICE hService;

	//Controle de acesso a lista
	static CMutex* mutex;

	//Flag que determina se a espera de uma trava deve ser cancelada
	static bool cancelWait;

public:
	static bool hasLock(HSERVICE hService);
	static bool canExecute(HSERVICE hService);
	static bool isLocked();
	static HWND getLockerWindowHandle(HSERVICE hService);
	static void setLockerWindowHandle(HSERVICE hService, HWND windowHandle);
	static int lock(HSERVICE hService, HWND windowHandle, long timeout);
	static bool unlock(HSERVICE hService);
	static bool grantNextLock();
	static bool isWaiting(HSERVICE service);
	static bool isInBounds(int index);
	static int addRequest(HSERVICE hService, HWND windowHandle);
	static bool removeRequest(HSERVICE hService);
	static void clearLock(int index);
	static void repackList();
	static bool isEmpty(int index);
	static DWORD getProcessID();
	static void setProcessID(DWORD pid);
	static HSERVICE getHService();
	static void setHService(HSERVICE service);
	static void exchange(int indexA, int indexB);


	//Armazenar o número máximo de elementos da lista lock
	static const int LIST_CAPACITY;
};

