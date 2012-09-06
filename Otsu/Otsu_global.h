#ifndef OTSU_GLOBAL_H
#define OTSU_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(OTSU_LIBRARY)
#  define OTSUSHARED_EXPORT Q_DECL_EXPORT
#else
#  define OTSUSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // OTSU_GLOBAL_H
