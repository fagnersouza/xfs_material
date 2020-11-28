// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "Sisemu.h"
#include <Windows.h>
#include <iostream>
#include "CRegistry.h"

using namespace std;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

DWORD extern WINAPI SisOpen(LPSTR lpszClientName) {
    
    wstring key_path = L"SOFTWARE\\WOW6432Node\\XFS\\PHYSICAL_SERVICES\\brxsisemu" + wstring(L"\\Open");

    HKEY hKey = CRegistry::Open(key_path.c_str(), KEY_QUERY_VALUE);

    if (hKey == NULL)
        return SISEMU_ERROR;

    CRegistry::Close(hKey);

    DWORD result = CRegistry::ReadValueDw(key_path.c_str(), TEXT("Result"));
    
    return result;
}

DWORD extern WINAPI SisClose() {
    wstring key_path = L"SOFTWARE\\WOW6432Node\\XFS\\PHYSICAL_SERVICES\\brxsisemu" + wstring(L"\\Close");

    DWORD result = CRegistry::ReadValueDw(key_path.c_str(), TEXT("Result"));

    return result;
}

DWORD extern WINAPI SisStatus(LPSISEMUSTATUS status) {
    DWORD result = SISEMU_ERROR;

    if (status == NULL)
        return result;

    wstring key_path = L"SOFTWARE\\WOW6432Node\\XFS\\PHYSICAL_SERVICES\\brxsisemu" + wstring(L"\\Status");

    result = CRegistry::ReadValueDw(key_path.c_str(), TEXT("Result"));

    if (result == SISEMU_ERROR)
        return SISEMU_ERROR;


    //Sensors
    wstring sensors = CRegistry::ReadValueStr(key_path.c_str(), TEXT("Sensors"));

    if (sensors.empty())
        return result;

    for (unsigned int i = 0; i < sensors.length(); i++) {
        status->Sensors[i] = (WORD)sensors[i];
    }

    //Auxiliares
    wstring auxiliares = CRegistry::ReadValueStr(key_path.c_str(), TEXT("Auxiliares"));

    if (auxiliares.empty())
        return result;

    for (unsigned int i = 0; i < auxiliares.length(); i++) {
        status->Auxiliaries[i] = (WORD)auxiliares[i];
    }

    //Doors
    wstring doors = CRegistry::ReadValueStr(key_path.c_str(), TEXT("Doors"));

    if (doors.empty())
        return result;

    for (unsigned int i = 0; i < doors.length(); i++) {
        status->Doors[i] = (WORD)doors[i];
    }

    //Guidlights
    wstring guidlights = CRegistry::ReadValueStr(key_path.c_str(), TEXT("Guidlights"));

    if (guidlights.empty())
        return result;

    for (unsigned int i = 0; i < guidlights.length(); i++) {
        status->Guidlights[i] = (WORD)guidlights[i];
    }

    //Indicators
    wstring indicators = CRegistry::ReadValueStr(key_path.c_str(), TEXT("Indicators"));

    if (indicators.empty())
        return result;

    for (unsigned int i = 0; i < indicators.length(); i++) {
        status->Indicators[i] = (WORD)indicators[i];
    }

    //Extra
    wstring extra = CRegistry::ReadValueStr(key_path.c_str(), TEXT("Extra"));

    if (extra.empty())
        return result;

    //string str((const char*)&extra, sizeof(wchar_t) / sizeof(char) * extra.size());
    //strcpy_s(cap->Extra, str.c_str());

    return result;
}

DWORD extern WINAPI SisCap(LPSISEMUCAP cap) {
    DWORD result = SISEMU_ERROR;
    
    if (cap == NULL)
        return result;

    wstring key_path = L"SOFTWARE\\WOW6432Node\\XFS\\PHYSICAL_SERVICES\\brxsisemu" + wstring(L"\\Capabilities");

    result = CRegistry::ReadValueDw(key_path.c_str(), TEXT("Result"));

    if (result == SISEMU_ERROR)
        return SISEMU_ERROR;


    //Sensors
    wstring sensors = CRegistry::ReadValueStr(key_path.c_str(), TEXT("Sensors"));

    if (sensors.empty())
        return result;
    
    for (unsigned int i = 0; i < sensors.length(); i++) {
        cap->Sensors[i] = (WORD)sensors[i];
    }

    //Auxiliares
    wstring auxiliares = CRegistry::ReadValueStr(key_path.c_str(), TEXT("Auxiliares"));

    if (auxiliares.empty())
        return result;

    for (unsigned int i = 0; i < auxiliares.length(); i++) {
        cap->Auxiliaries[i] = (WORD)auxiliares[i];
    }

    //Doors
    wstring doors = CRegistry::ReadValueStr(key_path.c_str(), TEXT("Doors"));

    if (doors.empty())
        return result;

    for (unsigned int i = 0; i < doors.length(); i++) {
        cap->Doors[i] = (WORD)doors[i];
    }

    //Guidlights
    wstring guidlights = CRegistry::ReadValueStr(key_path.c_str(), TEXT("Guidlights"));

    if (guidlights.empty())
        return result;

    for (unsigned int i = 0; i < guidlights.length(); i++) {
        cap->Guidlights[i] = (WORD)guidlights[i];
    }

    //Indicators
    wstring indicators = CRegistry::ReadValueStr(key_path.c_str(), TEXT("Indicators"));

    if (indicators.empty())
        return result;

    for (unsigned int i = 0; i < indicators.length(); i++) {
        cap->Indicators[i] = (WORD)indicators[i];
    }

    //Extra
    wstring extra = CRegistry::ReadValueStr(key_path.c_str(), TEXT("Extra"));

    if (extra.empty())
        return result;
    
    //string str((const char*)&extra, sizeof(wchar_t) / sizeof(char) * extra.size());
    //strcpy_s(cap->Extra, str.c_str());
    
    return result;
}

DWORD extern WINAPI SisReset() {
    wstring key_path = L"SOFTWARE\\WOW6432Node\\XFS\\PHYSICAL_SERVICES\\brxsisemu" + wstring(L"\\Reset");

    DWORD result = CRegistry::ReadValueDw(key_path.c_str(), TEXT("Result"));

    return result;
}

DWORD extern WINAPI SisSet(LPSISEMUSET set) {
    return SISEMU_ERROR;
}
