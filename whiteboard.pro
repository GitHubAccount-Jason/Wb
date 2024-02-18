######################################################################
# Automatically generated by qmake (3.1) Fri Jul 7 20:14:01 2023
######################################################################

TEMPLATE = app
TARGET = whiteboard
INCLUDEPATH += .

# You can make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# Please consult the documentation of the deprecated API in order to know
# how to port your code away from it.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_UP_TO=0x060000 # disables all APIs deprecated in Qt 6.0.0 and earlier
QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
# Input
HEADERS += mainwidget.h \
    floatingsetting.h \
    floatingwindow.h \
    settingspanel.h \
    wbdefs.h \
           whiteboard.h \

FORMS += mainwidget.ui \
    floatingsetting.ui \
    floatingwindow.ui \
    settingspanel.ui
SOURCES += main.cpp \
    floatingsetting.cpp \
    floatingwindow.cpp \
           mainwidget.cpp \
    settingspanel.cpp \
           whiteboard.cpp \

RESOURCES += source.qrc

include(QHotkey/qhotkey.pri)
LIBS += -L$$PWD/QHotkey/QHotkey
INCLUDEPATH += $$PWD/QHotkey/QHotkey
QMAKE_CXXFLAGS_RELEASE += -O3       # Release -O3
