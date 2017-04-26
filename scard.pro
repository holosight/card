QT += core
QT -= gui

CONFIG += c++11

TARGET = scard
CONFIG += console
CONFIG -= app_bundle

LIBS += -lwinscard

TEMPLATE = app

SOURCES += main.cpp \
    scard.cpp \
    scardcontext.cpp \
    pcsclog.cpp

HEADERS += \
    scard.h \
    scardcontext.h \
    pcsclog.h
