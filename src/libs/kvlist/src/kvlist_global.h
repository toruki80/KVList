#ifndef KVLIST_GLOBAL_H
#define KVLIST_GLOBAL_H

#include <QtCore/qglobal.h>
#include <QLoggingCategory>

// logging category we use for kvlist..
Q_DECLARE_LOGGING_CATEGORY(kvlist);

// shared library export
#if defined(KVLIST_LIBRARY)
#  define KVLIST_EXPORT Q_DECL_EXPORT
#else
#  define KVLIST_EXPORT Q_DECL_IMPORT
#endif



#endif // KVLIST_GLOBAL_H
