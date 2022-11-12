cmake_minimum_required(VERSION 3.16)
project(MashaCV VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

qt_standard_project_setup()

qt_add_executable(MashaCV WIN32 MACOSX_BUNDLE
    colordockwidget.cpp colordockwidget.h colordockwidget.ui
    graphicsview.cpp graphicsview.h graphicsview.ui
    imageprocessor.cpp imageprocessor.h
    layerdockwidget.cpp layerdockwidget.h layerdockwidget.ui
    layerlistmodel.cpp layerlistmodel.h
    main.cpp
    mainwindow.cpp mainwindow.h mainwindow.ui
    qgraphicsviewextended.cpp qgraphicsviewextended.h
    rangecontrol.cpp rangecontrol.h rangecontrol.ui
    resultsdockwidget.cpp resultsdockwidget.h resultsdockwidget.ui
    toolsdockwidget.cpp toolsdockwidget.h toolsdockwidget.ui
)
target_link_libraries(MashaCV PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Widgets
)


# Resources:
set(Resources_resource_files
    "ru.qm"
)

qt_add_resources(MashaCV "Resources"
    PREFIX
        "/languages"
    FILES
        ${Resources_resource_files}
)
set(Resources1_resource_files
    "icon.png"
)

qt_add_resources(MashaCV "Resources1"
    PREFIX
        "/icons"
    FILES
        ${Resources1_resource_files}
)

install(TARGETS MashaCV
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET MashaCV
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
