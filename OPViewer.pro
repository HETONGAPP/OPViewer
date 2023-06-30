QT += opcua widgets quick
CONFIG += c++11
DEPENDPATH += INCLUDEPATH

SOURCES += main.cpp \
    connectServer.cpp \
    opcuamodel.cpp \
    treeitem.cpp

HEADERS += \
    connectServer.h \
    opcuamodel.h \
    treeitem.h

RESOURCES += \
    qml/qml.qrc

