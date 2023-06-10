QT       += core httpserver
QT       -= gui

CONFIG += c++17

CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Database/coords.cpp \
    Database/routes_database.cpp \
    Database/structures.cpp \
    Engine/map.cpp \
    Engine/queries.cpp \
    Render/layers.cpp \
    Utils/json.cpp \
    main.cpp \
    travel_tracker.cpp

HEADERS += \
    Database/coords.h \
    Database/routes_database.h \
    Database/structures.h \
    Engine/graph.h \
    Engine/map.h \
    Engine/queries.h \
    Engine/router.h \
    Render/layers.h \
    Tests/profile.h \
    Tests/test_runner.h \
    Tests/tests.h \
    Utils/json.h \
    Utils/svg.h \
    travel_tracker.h

INCLUDEPATH += \
    Database/ \
    Utils/ \
    Engine/ \
    Render/ \
    Tests/

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
