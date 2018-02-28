#ifndef MFPI_GLOBAL_H
#define MFPI_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(MFPI_LIBRARY)
#  define MFPISHARED_EXPORT Q_DECL_EXPORT
#else
#  define MFPISHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // MFPI_GLOBAL_H
