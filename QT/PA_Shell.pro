#-------------------------------------------------
#
# Project created by QtCreator 2016-02-03T12:45:28
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = PA_Shell
TEMPLATE = app

win32:RC_ICONS += icon/lightning-bolt-th.ico


SOURCES += main.cpp\
        pa_shell.cpp \
    comportsettings.cpp \
    debugwindow.cpp \
    qcustomplot.cpp \
    thermostat.cpp

HEADERS  += pa_shell.h \
    comportsettings.h \
    debugwindow.h \
    mxusapi.h \
    qcustomplot.h \
    thermostat.h
	
FORMS    += pa_shell.ui \
    comportsettings.ui \
    helpdialog.ui \
    debugwindow.ui \
    thermostat.ui

RESOURCES += \
    res.qrc

LIBS += -L$$PWD/./ -lmxusapi

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.
