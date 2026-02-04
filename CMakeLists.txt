cmake_minimum_required(VERSION 3.14)
if(NOT DEFINED PRISM_VERSION)
    file(READ "${CMAKE_CURRENT_LIST_DIR}/vcpkg.json" _contents)
    string(JSON PRISM_VERSION GET "${_contents}" version)
endif()

project(prism_qt_ui VERSION ${PRISM_VERSION} LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Gui Widgets Quick Qml QuickControls2 ShaderTools  REQUIRED)
find_package(prism COMPONENTS container qt::modular qt::core)

option(PRISMUI_PCL "add pcl vtk " OFF)
option(PRISMUI_OPENGL  "using opengl" OFF)
option(PRISMUI_OPENGLES  "using opengl es" OFF)
option(PRISMUI_VULKAN  "using vulkan" OFF)
option(PRISMUI_RHI  "using rhi (qt6)" OFF)


if(PRISMUI_OPENGLES)
    add_compile_definitions(PRISMUI_OPENGLES)
    set(OPENGL_SOURCE
            src/controls/opengl/logorenderer.h
            src/controls/opengl/logorenderer.cpp
            src/controls/opengl/threadrenderer.h
            src/controls/opengl/threadrenderer.cpp
    )
endif()

if(PRISMUI_OPENGL)
    add_compile_definitions(PRISMUI_OPENGL)
    set(OPENGL_SOURCE
            src/controls/opengl/logorenderer.h
            src/controls/opengl/logorenderer.cpp
            src/controls/opengl/threadrenderer.h
            src/controls/opengl/threadrenderer.cpp
    )
endif()

if(PRISMUI_VULKAN)
    add_compile_definitions(PRISMUI_VULKAN)
    set(VULKAN_SOURCE
                src/controls/vulkan/vulkanCustomTextureItem.cpp
                src/controls/vulkan/vulkanCustomTextureItem.h
                src/controls/vulkan/vulkancustomtexturenode.h
                src/controls/vulkan/vulkancustomtexturenode.cpp
                src/controls/vulkan/vulkancustomtexture.h
                src/controls/vulkan/vulkancustomtexture.cpp
    )
    find_package(rs_vk_engine)
    set(VULKAN_LINKS
        rs_vk_engine
    )
endif()

if(PRISMUI_RHI)
    add_compile_definitions(PRISMUI_RHI)
    set(RHI_RENDERER_SOURCE
          src/controls/rhi/rhi_video_render.cpp
          src/controls/rhi/rhi_video_render.h
    )
    #find_package(rs_vk_engine)
    set(VULKAN_LINKS
    )

endif()


if(PRISMUI_VNCITEM)
    find_package(ZLIB REQUIRED )

    set(VNCITEM_SOURCE
        src/controls/vnc/vncitem.h
        src/controls/vnc/vncitem.cpp
    )

    set(VNCITME_LINKS
         ZLIB::ZLIB
    )


endif()

include_directories(${Qt${QT_VERSION_MAJOR}Quick_PRIVATE_INCLUDE_DIRS})
include_directories(${Qt${QT_VERSION_MAJOR}Core_PRIVATE_INCLUDE_DIRS})
include_directories(${Qt${QT_VERSION_MAJOR}Gui_PRIVATE_INCLUDE_DIRS})


if(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    add_definitions("-x objective-c++") # as you already have
    #set(CMAKE_EXE_LINKER_FLAGS "-framework ServiceManagement -framework Foundation -framework Cocoa -framework AppKit -framework Metal")
    set(CMAKE_SHARED_LINKER_FLAGS "-framework ServiceManagement -framework Foundation -framework Cocoa -framework AppKit  -framework Metal")
    set(MAC_SOURCE
        src/platform/os_macx/borderless_window_mac.h
        src/platform/os_macx/borderless_window_mac.mm )
elseif(WIN32)
    if(MSVC)
        add_definitions(-DWIN32MSVC)
        set(WIN32_LIBS  user32.lib dwmapi.lib)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
        add_definitions(-D_CRT_SECURE_NO_WARNINGS)
        set(MAC_SOURCE
            src/platform/os_win/borderless_window_win.cpp
            src/platform/os_win/borderless_window_win.h
            src/platform/os_win/borderless_window_helpe_win.cpp
            src/platform/os_win/borderless_window_helpe_win.h
            src/platform/os_win/multi_screen_helper_win.cpp
            src/platform/os_win/dpiscaleeventfilter.h src/platform/os_win/dpiscaleeventfilter.cpp
            src/platform/os_win/multi_screen_helper_win.h )
    else()
        add_definitions(-DWIN32GPP)
        set(WIN32_LIBS  user32 dwmapi)
    endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
endif()






add_library(${PROJECT_NAME} SHARED
  ${MAC_SOURCE}
  ${VNCITEM_SOURCE}
  include/prism/qt/ui/prismQt_ui_global.h
  include/prism/qt/ui/img_buffer_Info.h
  include/prism/qt/ui/helper/cpp_utility.h
  include/prism/qt/ui/model/roi_info.h

  src/prismQt_ui.h
  src/prismQt_ui.cpp
  src/prism_qt_ui.qrc
  src/platform/i_borderless_window_helper.h
  src/helper/cpp_utility.cpp
  src/helper/menuhelper.h
  src/helper/menuhelper.cpp
  src/helper/img_buffer_Info.cpp


  ${OPENGL_SOURCE}
  ${VULKAN_SOURCE}
  ${RHI_RENDERER_SOURCE}


  src/controls/window/qml_debug_window.h
  src/controls/window/qml_debug_window.cpp
  src/controls/forms/datepickercpp.h
  src/controls/forms/datepickercpp.cpp
  src/controls/shapes/video_roi_cpp.h
  src/controls/shapes/video_roi_cpp.cpp


  src/validator/datetime_validator.h
  src/validator/datetime_validator.cpp
)


add_library(prism::qt::ui  ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
)
target_compile_definitions(${PROJECT_NAME} PRIVATE PRISM_QT_UI_LIBRARY)


#上级目录
get_filename_component(PARENT_DIR "${PROJECT_SOURCE_DIR}" DIRECTORY)
target_compile_definitions(${PROJECT_NAME} PRIVATE PRISMQT_SOLUTION_SOURCEDIR="${PARENT_DIR}")


if(PRISMUI_PCL)
    find_package(VTK REQUIRED COMPONENTS GUISupportQtQuick)
    include("${VTK_USE_FILE}")
    target_compile_definitions(${PROJECT_NAME} PRIVATE PRISMUI_PCL)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        VTK::GUISupportQtQuick
    )
endif()

if(PRISMUI_RHI)
        qt6_add_shaders(prism_qt_ui "shaders"
            # gl_VertexId is in earliest OpenGL ES 3.00, and GLSL 130
            GLSL "300 es,130"
            PREFIX
                "/shader"
                ${PROJECT_SOURCE_DIR}/src/prism_qt_ui
            FILES
                "src/prism_qt_ui/Shaders/image_2d.vert"
                "src/prism_qt_ui/Shaders/mono8.frag"
                #"src/prism_qt_ui/Shaders/rgb24.frag"
                "src/prism_qt_ui/Shaders/rgba32.frag"
                #"src/prism_qt_ui/Shaders/bgr24.frag"
                "src/prism_qt_ui/Shaders/bgra32.frag"
                "src/prism_qt_ui/Shaders/nv12.frag"
                "src/prism_qt_ui/Shaders/yuv420p.frag"

        )
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${VULKAN_LINKS}
    prism::prism
    prism::container
    prism::qt::modular
    prism::qt::core
    ${WIN32_LIBS}
    ${VNCITME_LINKS}
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::OpenGL
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::Qml
    Qt${QT_VERSION_MAJOR}::QuickTemplates2Private
    Qt${QT_VERSION_MAJOR}::QuickControls2)


set(QML_IMPORT_PATH ${PROJECT_SOURCE_DIR}/src/prism_qt_ui CACHE STRING "QML Import Paths")


#
include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}_Targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

include(CMakePackageConfigHelpers)
write_basic_package_version_file("${PROJECT_NAME}ConfigVersion.cmake"
                                 VERSION ${PROJECT_VERSION}
                                 COMPATIBILITY SameMajorVersion)
configure_package_config_file("${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
                            "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        INSTALL_DESTINATION
        ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME})

install(EXPORT ${PROJECT_NAME}_Targets
        FILE ${PROJECT_NAME}Targets.cmake
        #NAMESPACE prism::
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME})

install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
              "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME})

    install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})

#

# vcpkg
if(EXISTS ${VCPKG_ROOT})
    execute_process( COMMAND git rev-parse --abbrev-ref HEAD WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} OUTPUT_VARIABLE GIT_BRANCH OUTPUT_STRIP_TRAILING_WHITESPACE RESULT_VARIABLE GIT_BRANCH_RESULT )
    execute_process( COMMAND git rev-parse HEAD WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} OUTPUT_VARIABLE GIT_HASH OUTPUT_STRIP_TRAILING_WHITESPACE )
    string(REPLACE "_" "-" PORT_NAME "${PROJECT_NAME}")
    configure_file(${PROJECT_SOURCE_DIR}/cmake/vcpkg.json.in ${VCPKG_ROOT}/ports/${PORT_NAME}/vcpkg.json @ONLY NEWLINE_STYLE LF)
    configure_file(${PROJECT_SOURCE_DIR}/cmake/vcpkg.json.in ${PROJECT_SOURCE_DIR}/vcpkg.json @ONLY NEWLINE_STYLE LF)
    configure_file(${PROJECT_SOURCE_DIR}/cmake/vcpkg_cus_info.json.in ${VCPKG_ROOT}/ports/${PORT_NAME}/vcpkg_cus_info.json @ONLY NEWLINE_STYLE LF)
    file(COPY ${PROJECT_SOURCE_DIR}/cmake/portfile.cmake DESTINATION ${VCPKG_ROOT}/ports/${PORT_NAME})
endif()



#execute_process(COMMAND ${CMAKE_COMMAND} -E echo "
#----------
#  ${MAC_SOURCE}
#----------" )
