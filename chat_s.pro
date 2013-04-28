#-------------------------------------------------
#
# Project created by QtCreator 2013-03-25T09:50:59
#
#-------------------------------------------------

QT       += core

QT       -= gui


DEPENDPATH += .
INCLUDEPATH += .

TARGET = chat_s
CONFIG   += console
CONFIG   -= app_bundle
QT  +=network

TEMPLATE = app


SOURCES += main.cpp \
    serverd.cpp \
    abstractchat.cpp

HEADERS += \
    serverd.h \
    abstractchat.h
