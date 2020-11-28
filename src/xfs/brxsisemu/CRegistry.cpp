#include "pch.h"
#include "CRegistry.h"

HKEY CRegistry::Open(LPCWSTR pcKey, DWORD access)
{
    HKEY  hKey;          // Recebe a handle de acesso para o registry
    DWORD dwSize = 0;    // Tamanho da chave criada

    RegCreateKeyEx(HKEY_LOCAL_MACHINE,
        pcKey,
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        access,
        NULL,
        &hKey,
        &dwSize);

    return(hKey);
}

void CRegistry::Close(HKEY hKey)
{
    RegCloseKey(hKey);
}

void CRegistry::WriteValueStr(LPCWSTR pcKey, LPCWSTR pcValue, char* pcData)
{
    HKEY  hKey;          // Recebe a handle de acesso para o registry

    hKey = Open(pcKey, KEY_SET_VALUE);
    RegSetValueEx(hKey,
        pcValue,
        0,
        REG_SZ,
        (const unsigned char*)(pcData),
        strlen(pcData) - 1);
    Close(hKey);
}

void CRegistry::WriteValueDw(LPCWSTR pcKey, LPCWSTR pcValue, DWORD pdwData)
{
    HKEY  hKey;          // Recebe a handle de acesso para o registry

    hKey = Open(pcKey, KEY_SET_VALUE);
    RegSetValueEx(hKey,
        pcValue,
        0,
        REG_DWORD,
        reinterpret_cast<BYTE*>(&pdwData),
        sizeof(pdwData));
    Close(hKey);
}

wstring CRegistry::ReadValueStr(LPCWSTR pcKey, LPCWSTR pcValue)
{
    HKEY  hKey;          // Recebe a handle de acesso para o registry
    TCHAR lcData[256];    // Valor que sera lido do registry
    DWORD ldwTipo = 0;   // Tipo
    DWORD ldwSize = 0;   // Tamanho

    memset(lcData, '\0', sizeof(lcData));
    ldwSize = sizeof(lcData);
    hKey = Open(pcKey, KEY_QUERY_VALUE);
    RegQueryValueEx(hKey,
        pcValue,
        0,
        &ldwTipo,
        (BYTE*)(&lcData), //(const BYTE *)(lcValor),
        &ldwSize);
    
    Close(hKey);

    wstring wstr(lcData);

    return wstr;
}

DWORD CRegistry::ReadValueDw(LPCWSTR pcKey, LPCWSTR pcValue)
{
    HKEY  hKey;              // Recebe a handle de acesso para o registry
    DWORD ldwData = 0;      // Valor que sera lido do registry
    DWORD ldwTipo = 0;      // Tipo
    DWORD ldwSize = 0;      // Tamanho

    ldwSize = sizeof(ldwData);
    hKey = Open(pcKey, KEY_QUERY_VALUE);
    RegQueryValueEx(hKey,
        pcValue,
        0,
        &ldwTipo,
        reinterpret_cast<BYTE*>(&ldwData),
        &ldwSize);
    Close(hKey);
    return(ldwData);
}
