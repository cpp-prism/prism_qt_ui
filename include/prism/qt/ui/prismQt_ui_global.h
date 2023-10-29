#ifndef PRISM_QT_UI_GLOBAL_H
#define PRISM_QT_UI_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(PRISM_QT_UI_LIBRARY)
#  define PRISMQT_UI_EXPORT Q_DECL_EXPORT
#else
#  define PRISMQT_UI_EXPORT Q_DECL_IMPORT
#endif

#endif // PRISM_QT_UI_GLOBAL_H
