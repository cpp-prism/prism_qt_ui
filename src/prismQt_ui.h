#ifndef PRISM_QT_UI_H
#define PRISM_QT_UI_H

#include <prism/qt/modular/interfaces/intf_module.h>
#include "include/prism/qt/ui/prismQt_ui_global.h"

namespace prism::qt::ui {


class PRISMQT_UI_EXPORT prismQt_ui : public prism::qt::modular::intfModule
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "prism::qt::modular::intfModule/1.0" FILE "prismQt_ui.json")
    Q_INTERFACES(prism::qt::modular::intfModule)
  public:
    prismQt_ui();
    ~prismQt_ui();

    // intfModule interface
  public:
    bool register_types() override;
    bool init() override;
    bool install() override;
    bool uninstall() override;
    bool uninit() override;
};

}//namespace prism::qt::ui

#endif // PRISM_QT_UI_H
