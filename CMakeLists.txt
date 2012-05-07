cmake_minimum_required(VERSION 2.6)
project(MashaCV)
find_package( OpenCV REQUIRED )
find_package( Qt4 REQUIRED )

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
SET(mashacv_SOURCES main.cpp mainwindow.cpp colorEdgesDetection.cpp hueWidget.cpp graphicsview.cpp image.cpp rangecontrol.cpp resultinfo.cpp)

set(QT_USE_QTGUI true)
set(QT_USE_QTCORE true) 
SET(moc_headers mainwindow.h hueWidget.h graphicsview.h rangecontrol.h resultinfo.h)
SET(mashacv_FORMS mainwindow.ui)
SET(mashacv_RESOURCES res.qrc)

QT4_WRAP_CPP(mashacv_headers_moc ${moc_headers})
QT4_WRAP_UI(mashacv_FORMS_HEADERS ${mashacv_FORMS})
QT4_ADD_RESOURCES(mashacv_RESOURCES_RCC ${mashacv_RESOURCES})


INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})


add_executable(mashacv ${mashacv_SOURCES} ${mashacv_headers_moc} ${mashacv_FORMS_HEADERS} ${mashacv_RESOURCES_RCC})
TARGET_LINK_LIBRARIES(mashacv ${QT_LIBRARIES})
target_link_libraries(mashacv ${OpenCV_LIBS})
