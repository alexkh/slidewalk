#-------------------------------------------------
#
# Project created by QtCreator 2012-07-13T10:32:04
#
#-------------------------------------------------

QT       += core gui opengl

TARGET = slidewalk
TEMPLATE = app


INCLUDEPATH += /usr/include/openslide
LIBS += -lopenslide


SOURCES += main.cpp\
        mainbox.cpp \
    view.cpp \
    zoomgraphicsview.cpp

HEADERS  += mainbox.hpp \
    view.hpp \
    zoomgraphicsview.hpp

FORMS    += mainbox.ui
