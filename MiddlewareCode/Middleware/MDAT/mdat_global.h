#ifndef MDAT_GLOBAL_H
#define MDAT_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(MDAT_LIBRARY)
#  define MDATSHARED_EXPORT Q_DECL_EXPORT
#else
#  define MDATSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // MDAT_GLOBAL_H
