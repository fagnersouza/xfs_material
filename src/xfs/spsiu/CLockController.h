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
	static HWND getLockerWindowHandle(HSERVICE hService);
};

