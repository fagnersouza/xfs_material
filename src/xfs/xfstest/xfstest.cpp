// xfstest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include<windows.h>
#include <iostream>
#include<XFSAPI.H>
#include<XFSADMIN.H>
#include<XFSCONF.H>
#include<brxutil.h>

int main()
{
    //std::cout << "Hello World!\n";

    //ETAPA 1
    
    HRESULT hResult;
    WFSVERSION xfs_version;
    WFSVERSION sp_version;
    WORD MENORVERSAO = 0X0B01;
    WORD MAIORVERSAO = 0x0A03;
                                //1.11  à 3.10 : range de versao do XFS Manager que a aplicacao suporta
    hResult = WFSStartUp(VERSAO(MENORVERSAO, MAIORVERSAO), &xfs_version);

    std::cout << "Result WFSStartUp: " << hResult << "\n";
    std::cout << "Description: " << xfs_version.szDescription << "\n";
    std::cout << "SystemStatus: " << xfs_version.szSystemStatus << "\n";
    std::cout << "HighVersion: " << std::hex << xfs_version.wHighVersion << "\n";
    std::cout << "LowVersion: " << std::hex << xfs_version.wLowVersion << "\n";
    std::cout << "Version: " << xfs_version.wVersion << "\n";
    std::cout << std::endl;


    //ETAPA 2
    HSERVICE hService;

    hResult = WFSOpen(
        TEXTO("Sensores"),
        WFS_DEFAULT_HAPP,
        TEXTO("XFSTest"),
        0,
        (5*1000),
        VERSAO(MENORVERSAO, MAIORVERSAO),//qual a versão de SPI a aplicação suporta
        &xfs_version,
        &sp_version,
        &hService
    );

    std::cout << "Result WFSOpen: " << std::dec << hResult << "\n";

    if (hResult == WFS_SUCCESS) {
        hResult = WFSClose(hService);
    }

    std::cout << std::endl;


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
