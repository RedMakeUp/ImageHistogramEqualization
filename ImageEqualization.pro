QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++17

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Tell the qcustomplot header that it will be used as library:
DEFINES += QCUSTOMPLOT_USE_LIBRARY

# Link with debug version of qcustomplot if compiling in debug mode, else with release library:
CONFIG(debug, release|debug) {
  win32:QCPLIB = qcustomplotd2
  else: QCPLIB = qcustomplotd
} else {
  win32:QCPLIB = qcustomplot2
  else: QCPLIB = qcustomplot
}
LIBS += -L$$PWD/libs -l$$QCPLIB
INCLUDEPATH += ./includes

SOURCES += \
    ImageDealer.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    ImageDealer.h \
    mainwindow.h \
    stb_image.h \
    stb_image_write.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

TARGET = sharedlib-usage
TEMPLATE = app

RESOURCES += \
    resource.qrc


