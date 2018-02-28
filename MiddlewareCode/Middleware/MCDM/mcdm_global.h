#ifndef MCDM_GLOBAL_H
#define MCDM_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(MCDM_LIBRARY)
#  define MCDMSHARED_EXPORT Q_DECL_EXPORT
#else
#  define MCDMSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // MCDM_GLOBAL_H
