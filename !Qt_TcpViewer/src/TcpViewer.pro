#-------------------------------------------------
#
# Project created by QtCreator
#
#-------------------------------------------------

QT += core gui widgets network

TARGET = TcpViewer

TEMPLATE = app

SOURCES += main.cpp \
    tcpviewer.cpp

HEADERS += tcpviewer.h

win32:RC_FILE = res.rc
