#-------------------------------------------------
#
# Project created by QtCreator 2012-03-19T22:08:10
#
#-------------------------------------------------

QT       += core gui

TARGET = qtIntegrity
TEMPLATE = app

INCLUDEPATH += ../../intelhex_class/

SOURCES += main.cpp\
        mainwindow.cpp \
    ../../intelhex_class/intelhexclass.cpp

HEADERS  += mainwindow.h \
        intelhexclass.h \
    ../../intelhexclass.h

RESOURCES += \
    qtIntegrity.qrc

RC_FILE = qtIntegrity.rc

OTHER_FILES += \
    qtIntegrity.rc \
    intelhexclass.ico

TRANSLATIONS += \
    qtIntegrity-de.ts
