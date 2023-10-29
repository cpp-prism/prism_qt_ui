#ifndef PRISM_QT_UI_I_BORDERLESS_WINDOW_HELPER_H
#define PRISM_QT_UI_I_BORDERLESS_WINDOW_HELPER_H

#include <QAbstractNativeEventFilter>
#include <QMap>
#include <QObject>
#include <QVariant>
#include <QWindow>

namespace prism::qt::ui {

class i_borderless_window_helper
{
  public:
    virtual void enableWindowBorderless([[maybe_unused]] qulonglong wId)
    {
    }
};

class i_borderless_window_native_event_fileter : public QAbstractNativeEventFilter
{
  public:
    virtual bool nativeEventFilter([[maybe_unused]] const QByteArray& eventType, [[maybe_unused]] void* message, [[maybe_unused]] long* result) override
    {
        return 0;
    }
    virtual ~i_borderless_window_native_event_fileter()
    {
    }

    QMap<quint64, QMap<QString, QVariant>> m_wndsParams;
};

}// namespace prism::qt::ui

#endif // PRISM_QT_UI_I_BORDERLESS_WINDOW_HELPER_H
