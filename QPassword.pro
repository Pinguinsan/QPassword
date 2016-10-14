#-------------------------------------------------
#
# Project created by QtCreator 2016-07-12T08:53:04
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QPassword
TEMPLATE = app

CONFIG += c++14

INCLUDEPATH += include/

LIBS += -ltjlutils

SOURCES += src/main.cpp \
           src/mainwindow.cpp

HEADERS  += include/mainwindow.h \

FORMS    += forms/mainwindow.ui

RESOURCES += resources/icons.qrc

