#ifndef WINDOWBORDERLESSHELPER_MACX_H
#define WINDOWBORDERLESSHELPER_MACX_H

#include "../IWindowBorderlessHelper.h"

class WindowBorderlessHelper_macx : public IWindowBorderlessHelper
{
  public:
    WindowBorderlessHelper_macx();

    // IWindowBorderlessHelper interface
  public:
    void enableWindowBorderless(qulonglong wId) override;
};

#endif // WINDOWBORDERLESSHELPER_MACX_H
