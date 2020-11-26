#include "CObjectContainer.h"
CObjectContainer::SessionList CObjectContainer::sessions;
CObjectContainer::LPCServiceProvider CObjectContainer::sp;

bool CObjectContainer::addSession(CSession* session)
{
    if (session != NULL && CObjectContainer::findSession(session->getHService()) == NULL) {
        CObjectContainer::sessions.push_back(session);
        return true;
    }

    return false;
}

CSession* CObjectContainer::findSession(HSERVICE hService)
{
    CSession* ret = NULL;

    CObjectContainer::SessionList::iterator it;

    for (it = CObjectContainer::sessions.begin(); it != CObjectContainer::sessions.end(); it++) {
        CSession* session = (*it);

        if (session->getHService() == hService) {
            ret = session;
            break;
        }
    }

    return ret;
}

CSession* CObjectContainer::removeSession(HSERVICE hService)
{
    //Tenta localizar a session
    CSession* session = CObjectContainer::findSession(hService);

    if (session != NULL) {
        CObjectContainer::sessions.remove(session);
    }
    
    return session;
}

CServiceProvider* CObjectContainer::getSP()
{
    if (sp == NULL)
        sp = new CServiceProvider();

    return sp;
}

void CObjectContainer::clearSP()
{
    delete sp;
    sp = NULL;
}
