// xfstest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include<windows.h>
#include <iostream>
#include<XFSAPI.H>
#include<XFSADMIN.H>
#include<XFSCONF.H>

#define TEXTO(x) const_cast<LPSTR>(x)

int main()
{
    std::cout << "Hello World!\n";

    //ETAPA 1
    
    HRESULT result;
    WFSVERSION xfs_version;
    WFSVERSION sp_version;

    result = WFSStartUp(MAKELONG(0X0003, 0X0003), &xfs_version);

    std::cout << "Result: " << result << "\n";
    std::cout << "Description: " << xfs_version.szDescription << "\n";
    std::cout << "SystemStatus: " << xfs_version.szSystemStatus << "\n";
    std::cout << "HighVersion: " << std::hex << xfs_version.wHighVersion << "\n";
    std::cout << "LowVersion: " << std::hex << xfs_version.wLowVersion << "\n";
    std::cout << "Version: " << xfs_version.wVersion << "\n";


    //ETAPA 2

    HSERVICE service;

    result = WFSOpen(
        TEXTO("Sensros"),
        WFS_DEFAULT_HAPP,
        TEXTO("XFSTest"),
        0,
        5000,
        MAKELONG(0X0003, 0X0303),
        &xfs_version,
        &sp_version,
        &service
    );

    std::cout << "Result: " << std::dec << result << "\n";


    //ETAPA 3
    WFSCleanUp();
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
