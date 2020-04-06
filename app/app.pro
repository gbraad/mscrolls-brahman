#
#  
#  
#    ______              _
#    | ___ \            | |
#    | |_/ / _ __  __ _ | |__   _ __ ___    __ _  _ __
#    | ___ \| '__|/ _` || '_ \ | '_ ` _ \  / _` || '_ \
#    | |_/ /| |  | (_| || | | || | | | | || (_| || | | |
#    \____/ |_|   \__,_||_| |_||_| |_| |_| \__,_||_| |_|
#  
#  
#    "The creative principle which lies realized in the whole world"
#  
#   Copyright (c) Strand Games 2018.
#  
#   This program is free software: you can redistribute it and/or modify it
#   under the terms of the GNU Lesser General Public License (LGPL) as published
#   by the Free Software Foundation, either version 3 of the License, or (at
#   your option) any later version.
#   
#   This program is distributed in the hope that it will be useful, but WITHOUT
#   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
#   FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
#   for more details.
#   
#   You should have received a copy of the GNU Lesser General Public License
#   along with this program. If not, see <http://www.gnu.org/licenses/>.
#  
#   contact@strandgames.com
#  
#  

TEMPLATE = app

QT += qml gui quick quickcontrols2 network multimedia

PROJ = $$PWD/..
include($$PROJ/app.pri)

DEFINES += STB_VORBIS_NO_PULLDATA_API STB_VORBIS_MAX_CHANNELS=2

### ////////////////// BASE ///////////////////////////////////////////

ios {
    OBJECTIVE_SOURCES += \
        iosappdelegate.mm \
        iosshare.mm
}

android {
  QT += androidextras

  #message("android plugins live in " $$[QT_INSTALL_PLUGINS])
}

INCLUDEPATH += $$PROJ/api $$RJSONDIR/include $$LIBPNG $$LIBZ

macx: !android {

        plugin.path = "$$PROJ/app/$$BUILDT/$$FTARGET/Contents/Frameworks"
        plugin.files = $$PROJ/app/$$BUILDT/libbrahmanapi.dylib
        plugin.files += $$PROJ/app/$$BUILDT/libbrahmanapi.1.dylib
        plugin.files += $$PROJ/app/$$BUILDT/libbrahmanapi.1.0.dylib
        plugin.files += $$PROJ/app/$$BUILDT/libbrahmanapi.1.0.0.dylib
        #plugin.files += $$PROJ/app/$$BUILDT/libz.dylib
        #plugin.files += $$PROJ/app/$$BUILDT/libz.1.dylib
        #plugin.files += $$PROJ/app/$$BUILDT/libz.1.0.dylib
        #plugin.files += $$PROJ/app/$$BUILDT/libz.1.0.0.dylib
        #plugin.files += $$PROJ/app/$$BUILDT/libpng.dylib
        #plugin.files += $$PROJ/app/$$BUILDT/libpng.1.dylib
        #plugin.files += $$PROJ/app/$$BUILDT/libpng.1.0.dylib
        #plugin.files += $$PROJ/app/$$BUILDT/libpng.1.0.0.dylib
        #plugin.files += $$PROJ/app/$$BUILDT/libbrahman.dylib
        INSTALLS += plugin

        plugin2.path = "$$PROJ/app/$$BUILDT/$$FTARGET/Contents/MacOS"
        plugin2.files = $$PROJ/app/$$BUILDT/libmagnetic.dylib
        #plugin2.files += $$PROJ/app/$$BUILDT/libz.dylib
        #plugin2.files += $$PROJ/app/$$BUILDT/libz.1.dylib
        #plugin2.files += $$PROJ/app/$$BUILDT/libz.1.0.dylib
        #plugin2.files += $$PROJ/app/$$BUILDT/libz.1.0.0.dylib
        #plugin2.files += $$PROJ/app/$$BUILDT/libpng.dylib
        #plugin2.files += $$PROJ/app/$$BUILDT/libpng.1.dylib
        #plugin2.files += $$PROJ/app/$$BUILDT/libpng.1.0.dylib
        #plugin2.files += $$PROJ/app/$$BUILDT/libpng.1.0.0.dylib
        INSTALLS += plugin2

        QMAKE_INFO_PLIST = macos/Info.plist

        QMAKE_LFLAGS_SONAME  = -Wl,-install_name,@executable_path/../Frameworks/
}

!ios {
    LIBS += -L$$PROJ/api/$$LIBDIR -lbrahmanapi
    LIBS += -L$$PROJ/ifi/$$LIBDIR -lifigame
    LIBS += -L$$LIBPNG/$$LIBDIR -lpng
    LIBS += -L$$LIBZ/$$LIBDIR -lz
}

unix:!macx {
 #LIBS -= -L$$LIBZ/$$LIBDIR -lz
  QMAKE_CFLAGS += $$LIBZ
  QMAKE_LFLAGS += -L$$LIBZ/$$LIBDIR -lz
}

ios {
    LIBS += -L$$PROJ/magnetic/$$LIBDIR/
    LIBS += -lmagnetic
    LIBS += -L$$PROJ/api/$$LIBDIR/
    LIBS += -lbrahmanapi
    LIBS += -L$$LIBPNG/$$LIBDIR -lpng
    LIBS += -L$$LIBZ/$$LIBDIR -lz
    LIBS += -L$$PROJ/ifi/$$LIBDIR -lifigame


    QMAKE_RPATHDIR = @executable_path/Frameworks
    #maglib.files += $$PROJ/magnetic/$$LIBDIR/libmagnetic.dylib
    #maglib.files += $$PROJ/brahman/$$LIBDIR/libbrahman.dylib
    #maglib.files += $$PROJ/api/$$LIBDIR/libbrahmanapi.dylib
    #maglib.path = Frameworks
    #QMAKE_BUNDLE_DATA += maglib
    enable_bitcode.name = "ENABLE_BITCODE"
    enable_bitcode.value = NO
    QMAKE_MAC_XCODE_SETTINGS += enable_bitcode
}
    
### ////////////////// SOURCES ///////////////////////////////////////////

SOURCES += main.cpp qlog.cpp qcontrol.cpp \
    iosappdelegate.mm \
    brahmanapp.cpp \
    apngreader.cpp \
    playsourceogg.cpp \
    vorbis.cpp \
    iosshare.mm
    
RESOURCES += qml.qrc

RESOURCES += icons/icons.qrc
OPTIONS += roboto

win32 {
   #generic icon, override in game specific pro file
   RC_ICONS = icons/brahman_logo.ico
}

include(qml-material/material.pri)

HEADERS += \
    qdefs.h \
    qlog.h \
    qcontrol.h \
    qprefs.h \
    updater.h \
    qtranscript.h \
    qconsole.h \
    qitems.h \
    qpeople.h \
    qmapbox.h \
    qshape.h \
    game.h \
    qmatcols.h \
    qwords.h \
    brahmanapp.h \
    iosshare.h \
    apngreader.h \
    qimgprov.h \
    qsbmodel.h \
    playsource.h \
    soundplayer.h \
    vorbis.h

DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat 

!ios {
    HEADERS -= iosshare.h
    SOURCES -= iosappdelegate.mm \
          iosshare.mm
}

