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
DEFINES += QT_LOGGING

debug {
  DEFINES += DEBUG
}

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        rule_edit_dialog.cpp \
        rules_dialog.cpp \
        settings_dialog.cpp \
        tags_dialog.cpp \
        ui_helper.cpp \
        ../hintergrund-cli/src/util/argument_parser.cpp \
        ../hintergrund-cli/src/util/config.cpp \
        ../hintergrund-cli/src/util/util.cpp \
        ../hintergrund-cli/src/tagging/tagger.cpp \
        ../hintergrund-cli/src/tagging/tag.cpp \
        ../hintergrund-cli/src/rules/rule.cpp \
        ../hintergrund-cli/src/rules/rule_date.cpp \
        ../hintergrund-cli/src/rules/rule_date_span.cpp \
        ../hintergrund-cli/src/rules/rule_io.cpp \
        ../hintergrund-cli/src/rules/rule_io_file.cpp \
        ../hintergrund-cli/src/rules/rule_io_stdin.cpp \
        ../hintergrund-cli/src/rules/rule_month_span.cpp \
        ../hintergrund-cli/src/rules/rule_set.cpp \
        ../hintergrund-cli/src/rules/rule_time_span.cpp \
        ../hintergrund-cli/src/rules/rule_weekday.cpp \
        ../hintergrund-cli/src/images/image.cpp \
        ../hintergrund-cli/src/images/image_library.cpp \
        ../hintergrund-cli/src/hintergrund_cli.cpp \
        ../hintergrund-cli/src/folder.cpp
HEADERS += \
        mainwindow.hpp \
        rule_edit_dialog.hpp \
        rules_dialog.hpp \
        settings_dialog.hpp \
        tags_dialog.hpp \
        ui_helper.hpp \
        ../hintergrund-cli/src/util/argument_parser.hpp \
        ../hintergrund-cli/src/util/config.hpp \
        ../hintergrund-cli/src/util/util.hpp \
        ../hintergrund-cli/src/tagging/tagger.hpp \
        ../hintergrund-cli/src/tagging/tag.hpp \
        ../hintergrund-cli/src/rules/rule.hpp \
        ../hintergrund-cli/src/rules/rule_date.hpp \
        ../hintergrund-cli/src/rules/rule_date_span.hpp \
        ../hintergrund-cli/src/rules/rule_io.hpp \
        ../hintergrund-cli/src/rules/rule_io_file.hpp \
        ../hintergrund-cli/src/rules/rule_io_stdin.hpp \
        ../hintergrund-cli/src/rules/rule_month_span.hpp \
        ../hintergrund-cli/src/rules/rule_set.hpp \
        ../hintergrund-cli/src/rules/rule_time_span.hpp \
        ../hintergrund-cli/src/rules/rule_weekday.hpp \
        ../hintergrund-cli/src/images/image.hpp \
        ../hintergrund-cli/src/images/image_library.hpp \
        ../hintergrund-cli/src/hintergrund_cli.hpp \
        ../hintergrund-cli/src/folder.hpp
FORMS += \
        mainwindow.ui \
        rule_edit_dialog.ui \
        rules_dialog.ui \
        settings_dialog.ui \
        tags_dialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

unix|win32: LIBS += -ljansson
