#ifndef DSND_GLOBAL_H
#define DSND_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(DSND_LIBRARY)
#  define DSNDSHARED_EXPORT Q_DECL_EXPORT
#else
#  define DSNDSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // DSND_GLOBAL_H
