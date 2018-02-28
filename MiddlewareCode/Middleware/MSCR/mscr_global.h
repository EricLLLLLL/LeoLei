#ifndef MSCR_GLOBAL_H
#define MSCR_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(MSCR_LIBRARY)
#  define MSCRSHARED_EXPORT Q_DECL_EXPORT
#else
#  define MSCRSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // MSCR_GLOBAL_H
