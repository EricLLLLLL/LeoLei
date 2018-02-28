#ifndef MTCP_GLOBAL_H
#define MTCP_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(MTCP_LIBRARY)
#  define MTCPSHARED_EXPORT Q_DECL_EXPORT
#else
#  define MTCPSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // MTCP_GLOBAL_H
