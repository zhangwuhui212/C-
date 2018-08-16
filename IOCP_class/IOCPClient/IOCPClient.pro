TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp

INCLUDEPATH += D:\QT_Work\boost\include\boost-1_58

LIBS += -LD:\QT_Work\boost\lib -lboost_system-mgw48-mt-d-1_58 -lboost_thread-mgw48-mt-d-1_58

LIBS += -LC:\Qt\Qt5.2.1\Tools\mingw48_32\i686-w64-mingw32\lib -lWs2_32

LIBS +=-lmswsock

LIBS += -lws2_32

