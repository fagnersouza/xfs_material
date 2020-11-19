#ifndef BRXUTIL_H
#define BRXUTIL_H

#include<Windows.h>
#include "XFSADMIN.H"
#include <stdio.h>
#include <stdarg.h>

#define TEXTO(x) const_cast<LPSTR>(x)
#define VERSAO(x, y) MAKELONG(x, y)
#define MAIORVERSAO(x) LOWORD(x)
#define MENORVERSAO(x) HIWORD(x)

void TRACE(const char* fmt, ...);


#endif //BRXUTIL_H