QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Connection.cpp \
    ConnectionManager.cpp \
    IOManager.cpp \
    Socket.cpp \
    TimeClock.cpp \
    WindowsSocketManager.cpp \
    configurationwindow.cpp \
    dialog.cpp \
    dialogmanager.cpp \
    dualdialog.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    Connection.h \
    ConnectionConfigurations.h \
    ConnectionManager.h \
    Definitions.h \
    IOManager.h \
    RoutineStructures.h \
    Socket.h \
    TimeClock.h \
    WSAEvents.h \
    WindowsSocketManager.h \
    WindowsThreadService.h \
    configurationwindow.h \
    dialog.h \
    dialogmanager.h \
    dualdialog.h \
    mainwindow.h

FORMS += \
    configurationwindow.ui \
    dialog.ui \
    dualdialog.ui \
    mainwindow.ui

LIBS += -lws2_32

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
