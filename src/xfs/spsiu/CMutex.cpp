#include "CMutex.h"

CMutex::CMutex()
{
	this->locked = false;
	this->ghMutex = NULL;
}

void CMutex::request()
{
	this->ghMutex = CreateMutex(
								NULL,	//atributos de segurança padrão
								FALSE,	//inializa sem owner
								NULL	//unnamed mutex
								);

	if (this->ghMutex == NULL)
		return;

	this->locked = true;

	WaitForSingleObject(this->ghMutex, INFINITE);
}

void CMutex::release()
{
	if (!ReleaseMutex(this->ghMutex))
		return;

	locked = false;
}

bool CMutex::isLocked()
{
	return this->locked;
}
