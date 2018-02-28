#ifndef MIDC_GLOBAL_H
#define MIDC_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(MIDC_LIBRARY)
#  define MIDCSHARED_EXPORT Q_DECL_EXPORT
#else
#  define MIDCSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // MIDC_GLOBAL_H
