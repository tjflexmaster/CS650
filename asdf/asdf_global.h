#ifndef ASDF_GLOBAL_H
#define ASDF_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(ASDF_LIBRARY)
#  define ASDFSHARED_EXPORT Q_DECL_EXPORT
#else
#  define ASDFSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // ASDF_GLOBAL_H
