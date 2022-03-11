QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
CONFIG += console

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

FORCE_DIMENSION_SDK_PATH = 'C:/Program Files/Force Dimension/sdk-3.14.0'
COM_ARDUINO_PATH = ../../../testComArduino/


INCLUDEPATH += $$PWD/include
INCLUDEPATH += $${COM_ARDUINO_PATH}
INCLUDEPATH += $${FORCE_DIMENSION_SDK_PATH}/include

DEPENDPATH += $${FORCE_DIMENSION_SDK_PATH}/include

SOURCES += \
    src/main.cpp \
    src/UserInterface.cpp \
    $${COM_ARDUINO_PATH}SerialPort.cpp \
    $${COM_ARDUINO_PATH}PC2Arduino.cpp \

HEADERS += \
    include/UserInterface.h\
    $${COM_ARDUINO_PATH}SerialPort.h \
    $${COM_ARDUINO_PATH}PC2Arduino.h \
    $${FORCE_DIMENSION_SDK_PATH}/include/dhdc.h \
    $${FORCE_DIMENSION_SDK_PATH}/include/drdc.h

FORMS += \
    UserInterface.ui

QMAKE_LFLAGS += /NODEFAULTLIB:libcmt
QMAKE_LFLAGS_WINDOWS += "/MANIFESTUAC:\"level='requireAdministrator' uiAccess='true'\""

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

unix:!macx|win32: LIBS += -L$${FORCE_DIMENSION_SDK_PATH}'/lib/' -ldhdms64 -ldrdms64






