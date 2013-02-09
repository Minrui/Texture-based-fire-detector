#-------------------------------------------------
#
# Project created by QtCreator 2011-01-26T11:10:01
#
#-------------------------------------------------

QT       += core gui network

TARGET = FireDetect
TEMPLATE = app


SOURCES += main.cpp\
        dialog.cpp \
    core/result.cpp \
    core/firedetector.cpp \
    core/jnode.cpp \
    core/glcmjnode.cpp \
    tinyxml/tinyxmlparser.cpp \
    tinyxml/tinyxmlerror.cpp \
    tinyxml/tinyxml.cpp \
    tinyxml/tinystr.cpp \
    core/jnodefactory.cpp \
    core/gaussbgjnode.cpp \
    core/rgbjnode.cpp \
    core/hsvjnode.cpp \
    core/mdnjnode.cpp \
    core/jset.cpp \
    core/streamserver.cpp \
    core/netcamera.cpp

HEADERS  += dialog.h \
    core/imagewrapper.h \
    core/result.h \
    core/firedetector.h \
    core/jnode.h \
    core/glcmjnode.h \
    tinyxml/tinyxml.h \
    tinyxml/tinystr.h \
    core/jnodefactory.h \
    core/gaussbgjnode.h \
    core/rgbjnode.h \
    core/hsvjnode.h \
    core/mdnjnode.h \
    core/jset.h \
    core/streamserver.h \
    core/netcamera.h

FORMS    += dialog.ui

CONFIG += link_pkgconfig

PKGCONFIG += opencv

RESOURCES += \
    res.qrc
