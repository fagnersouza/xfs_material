#include "CLockController.h"

const int CLockController::LIST_CAPACITY = 250;
DWORD CLockController::processID = 0;
HSERVICE CLockController::hService = 0;
bool CLockController::cancelWait = true;


#pragma data_seg("SHARED") //inicia area de memória compartilhada

DWORD requisitionListProcessID[CLockController::LIST_CAPACITY] = { 0 };
HWND requisitionListWindowHandle[CLockController::LIST_CAPACITY] = { NULL };
HSERVICE requisitionListHService[CLockController::LIST_CAPACITY] = { 0 };

int currentIndex = 0;

#pragma data_seg()
#pragma comment(linker, "/section:SHARED,RWS")


bool CLockController::hasLock(HSERVICE hService)
{
    return false;
}

bool CLockController::canExecute(HSERVICE hService)
{
    return ((!CLockController::isLocked()) || (CLockController::hasLock(hService)));
}

bool CLockController::isLocked()
{
    //Verifica se o indice atual é maior do que zero, indicando
    //que existem elementos no array de locks
    return (currentIndex > 0);
}

HWND CLockController::getLockerWindowHandle(HSERVICE hService) {
    HWND ret = NULL;
    
    if (CLockController::isLocked()) {
        ret = requisitionListWindowHandle[0];
    }

    return ret;
}

void CLockController::setLockerWindowHandle(HSERVICE hService, HWND windowHandle)
{
    if (CLockController::hasLock(hService)) {
        requisitionListWindowHandle[0] = windowHandle;
    }
}

int CLockController::lock(HSERVICE hService, HWND windowHandle, long timeout)
{
    //TODO: mutex
    HRESULT hResult = WFS_ERR_INTERNAL_ERROR;

    //Verifica se o processo requisitante não detém a trava atual ou se está esperando na fila
    if (!CLockController::hasLock(hService) && (!CLockController::isWaiting(hService))) {

        //Verificar se o índice disponível está dentro dos limites da lista
        if(CLockController::isInBounds(currentIndex)) {
            CLockController::addRequest(hService, windowHandle);
        }

        CLockController::cancelWait = false;
        long timerCounter = 0;

        do {
            if (requisitionListHService[0] == hService) {
                hResult = WFS_SUCCESS;
                break;
            }

            Sleep(100);

            timerCounter += 200;

            if ((timerCounter <= timeout) && (timeout != 0)) {
                CLockController::removeRequest(hService);
                hResult = WFS_ERR_TIMEOUT;
                break;
            }

        } while ((!CLockController::cancelWait));


        if (CLockController::cancelWait) {
            hResult = WFS_ERR_CANCELED;
            CLockController::removeRequest(hService);
        }
    }

    //TODO: mutex

    return hResult;
}

bool CLockController::unlock(HSERVICE hService) {
    bool ret = false;

    //Verifica se o processo corrente pode efetuar
    //a operação de destravamento
    if (CLockController::hasLock(hService)) {
        
        //Seta elemento de trava como nulo (sem dono)
        CLockController::clearLock(0);

        //Atribui o lock para o próximo elemento da lista (se houver)
        CLockController::grantNextLock();

        ret = true;
    }

    return ret;
}

bool CLockController::grantNextLock() {
    bool ret = false;

    //Verifica se exetem elementos no array
    if (currentIndex > 0) {
        //Limpa o atual detentor do lock
        CLockController::clearLock(0);

        //Rearranja os elementos do array
        for (int i = 1; i <= currentIndex; i++) {
            CLockController::exchange(i - 1, i);
        }

        //Zera ultimo elemento da lista
        CLockController::clearLock(currentIndex);
        currentIndex--;

        ret = true;        
    }

    return ret;
}

bool CLockController::isWaiting(HSERVICE service) {
    bool ret = false;

    //TODO mutex

    //Percorrer os elementos da lista até encontra service
    for (int i = 0; i <= currentIndex; i++) {
        if (requisitionListHService[i] == service) {
            ret = true;
            break;
        }
    }

    //TODO mutex

    return ret;
}

bool CLockController::isInBounds(int index)
{
    return ((index >= 0) && (index < CLockController::LIST_CAPACITY));
}

int CLockController::addRequest(HSERVICE hService, HWND windowHandle)
{
    int ret = -1;

    if (CLockController::isInBounds(currentIndex)) {
        requisitionListProcessID[currentIndex]      = CLockController::getProcessID();
        requisitionListWindowHandle[currentIndex]   = windowHandle;
        requisitionListHService[currentIndex]       = hService;

        ret = currentIndex;
        currentIndex++;
    }

    return ret;
}

bool CLockController::removeRequest(HSERVICE hService)
{
    bool ret = false;

    for (int i = 0; i <= currentIndex; i++) {
        if (requisitionListHService[i] == hService) {
            CLockController::clearLock(i);
            CLockController::repackList();
            currentIndex--;
            ret = true;
            break;
        }
    }
    
    return ret;
}

void CLockController::clearLock(int index) {
    requisitionListProcessID[index]     = 0;
    requisitionListWindowHandle[index]  = 0;
    requisitionListHService[index]      = 0;
}

void CLockController::repackList() {
    if ((!CLockController::isEmpty(0)) && (currentIndex < CLockController::LIST_CAPACITY)) {
        for (int i = 0; i <= currentIndex; i++) {
            if (CLockController::isEmpty(i)) {
                for (int j = i + 1; j < CLockController::LIST_CAPACITY; j++) {
                    CLockController::exchange(j - 1, j);
                }
            }
        }
    }
}

bool CLockController::isEmpty(int index) {
    return ((requisitionListHService[index] == 0) && (requisitionListProcessID[index] == 0) && (requisitionListWindowHandle[index] == 0));
}

void CLockController::exchange(int indexA, int indexB) {
    DWORD tmpProcessID      = requisitionListProcessID[indexA];
    HWND tmpWindowHandle    = requisitionListWindowHandle[indexA];
    HSERVICE tmpHService    = requisitionListHService[indexA];

    requisitionListProcessID[indexB]    = tmpProcessID;
    requisitionListWindowHandle[indexB] = tmpWindowHandle;
    requisitionListHService[indexB]     = tmpHService;
}

DWORD CLockController::getProcessID()
{
    return CLockController::processID;
}

void CLockController::setProcessID(DWORD pid)
{
    CLockController::processID = pid;
}

HSERVICE CLockController::getHService()
{
    return CLockController::hService;
}

void CLockController::setHService(HSERVICE service)
{
    CLockController::hService = service;
}
