#ifndef PRISM_QT_UI_ROI_INFO_H
#define PRISM_QT_UI_ROI_INFO_H

#include <prism/prism.hpp>
#include <prism/qt/core/hpp/prismQt.hpp>
#include "../prismQt_ui_global.h"
#include <prism/qt/core/hpp/prismModelProxy.hpp>
#include <prism/qt/core/hpp/prismQtJson.hpp>

#ifndef PRISM_EXPORT
#define PRISM_EXPORT  PRISMQT_UI_EXPORT
#endif
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

//PRISM_FIELDS(prism::qt::ui::model::ROI_info,offsetX,offsetY,width,height);
//PRISMQT_TYPE(prism::qt::ui::model::ROI_info);

namespace prism
{
namespace reflection
{
namespace privates
{
template <> struct st_field_do<prism::qt::ui::model::ROI_info>
{
    template <int BIS = 0, class LAM> constexpr static bool run(prism::qt::ui::model::ROI_info& model, const char* fname, LAM&& lambda, int& level)
    {
        ++level; using base_types = prism::reflection::privates::baseTypes<prism::qt::ui::model::ROI_info>::t; if constexpr (!std::is_same_v<base_types, void>)
        {
            bool hited = false; prism::reflection::privates::for_each_bases<base_types>([&](auto* base_ptr_null)
            {
                using b_t = std::remove_pointer_t<decltype(base_ptr_null)>; b_t& baseRef = static_cast<b_t&>(model); hited = st_field_do<b_t>::template run<BIS>(baseRef, fname, lambda, level); --level; if (hited) return true; return false;
            }
            ); if (hited) return true;
        }
        switch (prism::utilities::const_hash(fname))
        {
        case prism::utilities::const_hash("offsetX"): if constexpr (!prism::attributes::privates::is_field_ignore<prism::qt::ui::model::ROI_info, decltype(prism::utilities::getMT(&prism::qt::ui::model::ROI_info::offsetX)), &prism::qt::ui::model::ROI_info::offsetX, BIS>())
            {
                lambda(model.offsetX); return true;
            }
            break; case prism::utilities::const_hash("offsetY"): if constexpr (!prism::attributes::privates::is_field_ignore<prism::qt::ui::model::ROI_info, decltype(prism::utilities::getMT(&prism::qt::ui::model::ROI_info::offsetY)), &prism::qt::ui::model::ROI_info::offsetY, BIS>())
            {
                lambda(model.offsetY); return true;
            }
            break; case prism::utilities::const_hash("width"): if constexpr (!prism::attributes::privates::is_field_ignore<prism::qt::ui::model::ROI_info, decltype(prism::utilities::getMT(&prism::qt::ui::model::ROI_info::width)), &prism::qt::ui::model::ROI_info::width, BIS>())
            {
                lambda(model.width); return true;
            }
            break; case prism::utilities::const_hash("height"): if constexpr (!prism::attributes::privates::is_field_ignore<prism::qt::ui::model::ROI_info, decltype(prism::utilities::getMT(&prism::qt::ui::model::ROI_info::height)), &prism::qt::ui::model::ROI_info::height, BIS>())
            {
                lambda(model.height); return true;
            }
            break;
        }
        return false;
    }

}
;
}

}

}
namespace prism
{
namespace attributes
{
namespace privates
{
template <class AT> struct st_field_attribute_do<prism::qt::ui::model::ROI_info, AT>
{
    template <class LAM> constexpr static void run(const char* fname, LAM&& lambda)
    {
        switch (prism::utilities::const_hash(fname))
        {
        case prism::utilities::const_hash("offsetX"): lambda(prism::attributes::privates::field_attribute<prism::qt::ui::model::ROI_info, decltype(prism::utilities::getMT(&prism::qt::ui::model::ROI_info::offsetX)), &prism::qt::ui::model::ROI_info::offsetX, AT>::value()); break; case prism::utilities::const_hash("offsetY"): lambda(prism::attributes::privates::field_attribute<prism::qt::ui::model::ROI_info, decltype(prism::utilities::getMT(&prism::qt::ui::model::ROI_info::offsetY)), &prism::qt::ui::model::ROI_info::offsetY, AT>::value()); break; case prism::utilities::const_hash("width"): lambda(prism::attributes::privates::field_attribute<prism::qt::ui::model::ROI_info, decltype(prism::utilities::getMT(&prism::qt::ui::model::ROI_info::width)), &prism::qt::ui::model::ROI_info::width, AT>::value()); break; case prism::utilities::const_hash("height"): lambda(prism::attributes::privates::field_attribute<prism::qt::ui::model::ROI_info, decltype(prism::utilities::getMT(&prism::qt::ui::model::ROI_info::height)), &prism::qt::ui::model::ROI_info::height, AT>::value()); break;
        }

    }

}
;
}

}

}
namespace prism
{
namespace reflection
{
namespace privates
{
template <> struct st_visit_fields<prism::qt::ui::model::ROI_info>
{
    template <int BIS, class Obj, class Vis> constexpr static void run(Obj&& model, Vis&& visitor)
    {
        if constexpr (!prism::attributes::privates::is_field_ignore<prism::qt::ui::model::ROI_info, decltype(prism::utilities::getMT(&prism::qt::ui::model::ROI_info::offsetX)), &prism::qt::ui::model::ROI_info::offsetX, BIS>()) visitor("offsetX", model.offsetX); if constexpr (!prism::attributes::privates::is_field_ignore<prism::qt::ui::model::ROI_info, decltype(prism::utilities::getMT(&prism::qt::ui::model::ROI_info::offsetY)), &prism::qt::ui::model::ROI_info::offsetY, BIS>()) visitor("offsetY", model.offsetY); if constexpr (!prism::attributes::privates::is_field_ignore<prism::qt::ui::model::ROI_info, decltype(prism::utilities::getMT(&prism::qt::ui::model::ROI_info::width)), &prism::qt::ui::model::ROI_info::width, BIS>()) visitor("width", model.width); if constexpr (!prism::attributes::privates::is_field_ignore<prism::qt::ui::model::ROI_info, decltype(prism::utilities::getMT(&prism::qt::ui::model::ROI_info::height)), &prism::qt::ui::model::ROI_info::height, BIS>()) visitor("height", model.height);
    }

}
; template <> struct st_for_each_fields<prism::qt::ui::model::ROI_info>
{
    template <int BIS = 0, class LAM> constexpr static void run(prism::qt::ui::model::ROI_info& model, LAM&& lambda)
    {
        using base_types = prism::reflection::privates::baseTypes<prism::qt::ui::model::ROI_info>::t; if constexpr (!std::is_same_v<base_types, void>)
        {
            prism::reflection::privates::for_each_bases<base_types>([&](auto* base_ptr_null)
            {
                using b_t = std::remove_pointer_t<decltype(base_ptr_null)>; b_t& baseRef = static_cast<b_t&>(model); prism::reflection::for_each_fields<BIS>(baseRef, lambda); return false;
            }
            );
        }
        st_visit_fields<prism::qt::ui::model::ROI_info>::run<BIS>(std::move(model), std::move(lambda));
    }

}
;
}
template <> constexpr PRISM_EXPORT bool has_md<prism::qt::ui::model::ROI_info>()
{
    return true;
}

}

}
;


template class PRISM_EXPORT std::stdoptional<prism::qt::ui::model::ROI_info>;;
template class PRISM_EXPORT std::shared_ptr<prism::qt::ui::model::ROI_info>;
template class PRISM_EXPORT std::list<prism::qt::ui::model::ROI_info>;
//template class PRISM_EXPORT QList<prism::qt::ui::model::ROI_info>;
template class PRISM_EXPORT std::vector<prism::qt::ui::model::ROI_info>;
template class PRISM_EXPORT QVector<prism::qt::ui::model::ROI_info>;
template class PRISM_EXPORT std::shared_ptr<prism::qt::core::prismEnumProxy<prism::qt::ui::model::ROI_info>>;
template class PRISM_EXPORT std::shared_ptr<prism::qt::core::prismModelProxy<prism::qt::ui::model::ROI_info>>;
template class PRISM_EXPORT std::shared_ptr<prism::qt::core::prismModelListProxy<prism::qt::ui::model::ROI_info>>;
template class PRISM_EXPORT std::shared_ptr<prism::qt::core::prismTreeNodeProxy<prism::qt::ui::model::ROI_info>>;
template class PRISM_EXPORT std::shared_ptr<prism::qt::core::prismTreeModelProxy<prism::qt::ui::model::ROI_info>>;;


Q_DECLARE_METATYPE(prism::qt::ui::model::ROI_info)
Q_DECLARE_METATYPE(prism::qt::ui::model::ROI_info*)
Q_DECLARE_METATYPE(std::stdoptional<prism::qt::ui::model::ROI_info>)
Q_DECLARE_METATYPE(std::shared_ptr<prism::qt::ui::model::ROI_info>)
Q_DECLARE_METATYPE(std::list<prism::qt::ui::model::ROI_info>)
Q_DECLARE_METATYPE(QList<prism::qt::ui::model::ROI_info>)
Q_DECLARE_METATYPE(std::vector<prism::qt::ui::model::ROI_info>)
Q_DECLARE_METATYPE(QVector<prism::qt::ui::model::ROI_info>)
Q_DECLARE_METATYPE(prism::qt::core::prismEnumProxy<prism::qt::ui::model::ROI_info>*)
Q_DECLARE_METATYPE(std::shared_ptr<prism::qt::core::prismEnumProxy<prism::qt::ui::model::ROI_info>>)
Q_DECLARE_METATYPE(prism::qt::core::prismModelProxy<prism::qt::ui::model::ROI_info>*)
Q_DECLARE_METATYPE(std::shared_ptr<prism::qt::core::prismModelProxy<prism::qt::ui::model::ROI_info>>)
Q_DECLARE_METATYPE(prism::qt::core::prismModelListProxy<prism::qt::ui::model::ROI_info>*)
Q_DECLARE_METATYPE(std::shared_ptr<prism::qt::core::prismModelListProxy<prism::qt::ui::model::ROI_info>>)
Q_DECLARE_METATYPE(prism::qt::core::prismTreeNodeProxy<prism::qt::ui::model::ROI_info>*)
Q_DECLARE_METATYPE(std::shared_ptr<prism::qt::core::prismTreeNodeProxy<prism::qt::ui::model::ROI_info>>)
Q_DECLARE_METATYPE(prism::qt::core::prismTreeModelProxy<prism::qt::ui::model::ROI_info>*)
Q_DECLARE_METATYPE(std::shared_ptr<prism::qt::core::prismTreeModelProxy<prism::qt::ui::model::ROI_info>>);

namespace prism
{
namespace json
{
namespace privates
{
template struct PRISM_EXPORT prism::json::privates::jsonValue<prism::qt::ui::model::ROI_info>;
}
}
}

namespace prism
{
namespace attributes
{
namespace privates
{
template <> constexpr inline PRISM_EXPORT bool is_field_ignore<prism::qt::ui::model::ROI_info, decltype(prism::utilities::getMT(&prism::qt::ui::model::ROI_info::offsetY)), &prism::qt::ui::model::ROI_info::offsetY, prism::utilities::const_hash("json")>()
{
    return true;
}

}

}

}



//PRISM_FIELD_ATTRIBUTE(prism::json::attributes::Attr_json_alias,&prism::qt::ui::model::ROI_info::offsetX, "offsetX_alias")
namespace prism
{
namespace attributes
{
namespace privates
{
template <>  struct field_attribute<decltype(prism::utilities::getT(std::declval<decltype(&prism::qt::ui::model::ROI_info::offsetX)>())), decltype(prism::utilities::getMT(std::declval<decltype(&prism::qt::ui::model::ROI_info::offsetX)>())), &prism::qt::ui::model::ROI_info::offsetX, prism::json::attributes::Attr_json_alias>
{
    constexpr static std::stdoptional<prism::json::attributes::Attr_json_alias::value_type> value()
    {
        return "offsetX_alias";
    }

}
;
template PRISM_EXPORT  struct field_attribute<decltype(prism::utilities::getT(std::declval<decltype(&prism::qt::ui::model::ROI_info::offsetX)>())), decltype(prism::utilities::getMT(std::declval<decltype(&prism::qt::ui::model::ROI_info::offsetX)>())), &prism::qt::ui::model::ROI_info::offsetX, prism::json::attributes::Attr_json_alias>;
}

}

}




#undef PRISM_EXPORT
#endif // PRISM_QT_UI_ROI_INFO_H

