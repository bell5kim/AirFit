#-------------------------------------------------
#
# Project created by QtCreator 2015-03-06T08:28:42
#
#-------------------------------------------------

QT       += core gui printsupport xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AirFit
TEMPLATE = app

INCLUDEPATH += /usr/local/qwt-6.1.2/include/
LIBS += -L/usr/local/qwt-6.1.2/lib/ -lqwt

SOURCES += main.cpp\
        airfit.cpp \
    checkspinsliderbox.cpp \
    AirFitInput.cpp \
    myplot.cpp

HEADERS  += airfit.h \
            cplx.h \
            pixmaps.h \
    checkspinsliderbox.h \
    AirFitInput.h \
    myplot.h

FORMS    += airfit.ui \
    checkspinsliderbox.ui


CONFIG   += qwt
