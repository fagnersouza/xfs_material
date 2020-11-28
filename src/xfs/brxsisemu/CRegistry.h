#pragma once
#include <windows.h>
#include <stdio.h>
#include <string>

#define KEY_PATH "SOFTWARE\\WOW6432Node\\XFS\\PHYSICAL_SERVICES\\brxsisemu";

using namespace std;

class CRegistry
{
public:
	static HKEY Open(LPCWSTR pcKey, DWORD access);
	static void Close(HKEY hKey);
	static void WriteValueStr(LPCWSTR pcKey, LPCWSTR pcValue, char* pcData);
	static void WriteValueDw(LPCWSTR pcKey, LPCWSTR pcValue, DWORD pdwData);
	static wstring ReadValueStr(LPCWSTR pcKey, LPCWSTR pcValue);
	static DWORD ReadValueDw(LPCWSTR pcKey, LPCWSTR pcValue);
};

