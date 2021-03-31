#-------------------------------------------------
#
# Project created by QtCreator 2021-02-08T11:23:46
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WriteDisk
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    iic_pthread.cpp \
    page_pthread.cpp \
    led.cpp

HEADERS  += widget.h \
    i2c-dev.h \
    iic_pthread.h \
    page_pthread.h \
    led.h
