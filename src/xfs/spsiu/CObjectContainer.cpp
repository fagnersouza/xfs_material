#include "CObjectContainer.h"
CObjectContainer::SessionList CObjectContainer::sessions;

bool CObjectContainer::addSession(CSession* session)
{
    if (session != NULL && CObjectContainer::findSession(session->getHService() == NULL)) {
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
