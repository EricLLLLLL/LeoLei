#ifndef MCIM_GLOBAL_H
#define MCIM_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(MCIM_LIBRARY)
#  define MCIMSHARED_EXPORT Q_DECL_EXPORT
#else
#  define MCIMSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // MCIM_GLOBAL_H
