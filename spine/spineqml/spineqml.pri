
#CONFIG += qt 
#QT += qml quick

SOURCES += \
    $$PWD/spineqml.cpp \
    $$PWD/texture.cpp \
    $$PWD/skeletonrenderer.cpp \
    $$PWD/rendercmdscache.cpp \
    $$PWD/skeletonanimationfbo.cpp \
    $$PWD/spineevent.cpp

HEADERS += \
    $$PWD/spineqml.h \
    $$PWD/texture.h \
    $$PWD/skeletonrenderer.h \
    $$PWD/rendercmdscache.h \
    $$PWD/skeletonanimationfbo.h \
    $$PWD/spineevent.h

LIBS += -L$$PWD/../spine-c/$$BUILDT/ -lspine

win32 {
    LIBS += -lopengl32 -lglu32
}

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/../spine-c/include/
#DEPENDPATH += $$PWD/../spine-c/include/

#qml_folder.source = $$PWD/Spine
#qml_folder.target = .
#DEPLOYMENTFOLDERS += qml_folder

RESOURCES += $$PWD/resource.qrc


