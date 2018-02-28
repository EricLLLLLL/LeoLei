#ifndef TRACETESTDLL1_H
#define TRACETESTDLL1_H

#include "tracetestdll1_global.h"

class TRACETESTDLL1SHARED_EXPORT TraceTestDll1
{

public:
    TraceTestDll1();
};

extern "C" {
    TRACETESTDLL1SHARED_EXPORT long userLog2();
}


#endif // TRACETESTDLL1_H
