#ifndef TRACETESTDLL_H
#define TRACETESTDLL_H

#include "tracetestdll_global.h"
#include "easytrace.h"

class TRACETESTDLLSHARED_EXPORT TraceTestDll
{

public:
    TraceTestDll();
    static TraceTestDll* GetInstance();

private:
    static TraceTestDll* m_pInstance;
};

extern "C" {
    TRACETESTDLLSHARED_EXPORT long userLog1();
}


#endif // TRACETESTDLL_H
