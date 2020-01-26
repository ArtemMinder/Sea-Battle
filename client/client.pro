#-------------------------------------------------
#
# Project created by QtCreator 2012-06-21T19:04:16
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = client
TEMPLATE = app

CONFIG += static

SOURCES += main.cpp\
    Connect.cpp \
    Field.cpp \
    Game.cpp \
    Logic.cpp \
    Textures.cpp \
        mainwindow.cpp

HEADERS  += mainwindow.h \
    Connect.h \
    Field.h \
    Game.h \
    Logic.h \
    Textures.h

FORMS    += mainwindow.ui

RESOURCES += \
    resources.qrc
