#-------------------------------------------------
#
# Project created by QtCreator 2019-05-28T20:46:00
#
#-------------------------------------------------

QT       += core gui
INCLUDEPATH += "../hintergrund-cli/src"
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = hintergrund-ui
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        ui_helper.cpp \
        ../hintergrund-cli/src/util/*.cpp \
        ../hintergrund-cli/src/tagging/*.cpp \
        ../hintergrund-cli/src/rules/*.cpp \
        ../hintergrund-cli/src/images/*.cpp \
        ../hintergrund-cli/src/*.cpp
HEADERS += \
        mainwindow.hpp \
        ui_helper.hpp \
        ../hintergrund-cli/src/util/*.hpp \
        ../hintergrund-cli/src/tagging/*.hpp \
        ../hintergrund-cli/src/rules/*.hpp \
        ../hintergrund-cli/src/images/*.hpp \
        ../hintergrund-cli/src/*.hpp
FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

unix|win32: LIBS += -ljansson
