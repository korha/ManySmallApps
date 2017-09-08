#-------------------------------------------------
#
# Project created by QtCreator
#
#-------------------------------------------------

QT += core network
QT -= gui

TARGET = PortMapProxy

TEMPLATE = app

CONFIG += console

SOURCES += main.cpp \
    portmapproxy.cpp

HEADERS += portmapproxy.h

win32:RC_FILE = res.rc
