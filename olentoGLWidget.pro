#-------------------------------------------------
#
# Project created by QtCreator 2016-05-12T10:45:00
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = olentoGLWidget
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    dobject.cpp \
    glwidget.cpp \
    oLoader.cpp \
    window.cpp

HEADERS  += mainwindow.h \
    dClock.h \
    dFace.h \
    dobject.h \
    glwidget.h \
    oLoader.h \
    window.h

FORMS    += mainwindow.ui
