#ifndef PRISM_QT_UI_ROI_INFO_H
#define PRISM_QT_UI_ROI_INFO_H


#include "../prismQt_ui_global.h"
#ifndef PRISM_EXPORT
#define PRISM_EXPORT PRISMQT_UI_EXPORT

#include <prism/prism.hpp>
#include <prism/prismSql.hpp>
#include <prism/qt/core/hpp/prismQt.hpp>
#include <prism/qt/core/hpp/prismModelProxy.hpp>
#include <prism/qt/core/hpp/prismQtJson.hpp>




namespace prism::qt::ui::model
{

struct PRISMQT_UI_EXPORT ROI_info
{
    int offsetX = 0;
    int offsetY = 0;
    int width = 100;
    int height = 100;
    bool operator ==(const ROI_info& other) const
    {
        return this->offsetX == other.offsetX &&
         this->offsetY == other.offsetY &&
         this->width == other.width &&
         this->height == other.height ;

    }
    bool operator <([[maybe_unused]]const ROI_info& other) const
    {
        return false;
    }
};

}// namespace prism::qt::ui::model



PRISM_FIELDS(prism::qt::ui::model::ROI_info,offsetX,offsetY,width,height);
PRISMQT_CLASS(prism::qt::ui::model::ROI_info)





#endif //PRISM_EXPORT
#endif // PRISM_QT_UI_ROI_INFO_H

