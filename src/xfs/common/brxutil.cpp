#include <brxutil.h>

void TRACE(const char* fmt, ...) {
    char aux[256];
    memset(aux, 0x00, sizeof(aux));
    
    va_list args;
    va_start(args, fmt);
    vsprintf_s(aux, fmt, args);
    va_end(args);

    WFMOutputTraceData(TEXTO(aux));
}
