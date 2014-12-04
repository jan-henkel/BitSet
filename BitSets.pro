#-------------------------------------------------
#
# Project created by QtCreator 2014-12-01T00:53:15
#
#-------------------------------------------------

QT       += core gui
QMAKE_CXXFLAGS += -std=c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BitSets
TEMPLATE = app


SOURCES += main.cpp\
        bitsetsmainwindow.cpp \
    bitset.cpp \
    consoleinputtextedit.cpp

HEADERS  += bitsetsmainwindow.h \
    bitset.h \
    consoleinputtextedit.h

FORMS    += bitsetsmainwindow.ui
