#ifndef WINDOWBORDERLESSHELPER_MACX_H
#define WINDOWBORDERLESSHELPER_MACX_H

#include "../i_borderless_window_helper.h"

namespace prism::qt::ui
{
class borderless_window_mac : public i_borderless_window_helper
{
  public:
    borderless_window_mac();

    // IWindowBorderlessHelper interface
  public:
    void enableWindowBorderless(qulonglong wId) override;
};
} // namespace prism::qt::ui

#endif // WINDOWBORDERLESSHELPER_MACX_H
