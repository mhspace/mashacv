#-------------------------------------------------
#
# Project created by QtCreator 2012-10-17T19:54:21
#
#-------------------------------------------------

QT       += core gui

#LIBS += -lkdeui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MashaCV
TEMPLATE = app
#QMAKE_CXXFLAGS_RELEASE -= -O2
#QMAKE_CXXFLAGS_RELEASE += -O3 -funroll-all-loops -ftree-vectorize -msse -msse2 -msse3 -mfpmath=sse


SOURCES += main.cpp\
        mainwindow.cpp \
    graphicsview.cpp \
    imageprocessor.cpp \
    colordockwidget.cpp \
    qgraphicsviewextended.cpp \
    layerdockwidget.cpp \
    layerlistmodel.cpp \
    resultsdockwidget.cpp \
    toolsdockwidget.cpp

HEADERS  += mainwindow.h \
    graphicsview.h \
    imageprocessor.h \
    colordockwidget.h \
    qgraphicsviewextended.h \
    layerdockwidget.h \
    layerlistmodel.h \
    resultsdockwidget.h \
    toolsdockwidget.h

FORMS    += mainwindow.ui \
    graphicsview.ui \
    colordockwidget.ui \
    layerdockwidget.ui \
    resultsdockwidget.ui \
    toolsdockwidget.ui

RESOURCES += \
    Resources.qrc
TRANSLATIONS = ru.ts
