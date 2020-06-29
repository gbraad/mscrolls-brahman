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
#   Copyright (c) Voidware 2016-2017.
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
#   contact@voidware.com
#  
#  

TEMPLATE=lib
TARGET=magnetic
CONFIG += plugin
QT += core
#QT -= gui

#VERSION = 1.2.3

PROJ = $$PWD/..
include($$PROJ/app.pri)

### ////////////////// BASE ///////////////////////////////////////////

gcc:QMAKE_CFLAGS_WARN_ON = -Wall -Wno-unused-parameter -Wno-unknown-pragmas -Wno-unused-result -Wno-switch -Wno-parentheses -Wno-pointer-sign 
LIBKL = $$PWD/kl
         
INCLUDEPATH += $$PROJ/common $$PROJ/api $$PROJ/app $$LIBPNG $$LIBZ 
DEFINES += LOG_MT 

ios {
    CONFIG += static
    #CONFIG += shared
}

#PRE_TARGETDEPS += $$LIBKL/$$LIBDIR/libkl.a

LIBS += -L$$LIBPNG/$$LIBDIR -lpng
LIBS += -L$$LIBZ/$$LIBDIR -lz
#LIBS += -L$$LIBKL/$$LIBDIR -lkl

unix:!android {
    DESTDIR=$$BUILDT
    OBJECTS_DIR = $$BUILDT
    MOC_DIR = $$BUILDT
}

win32:!msvc {

   QMAKE_POST_LINK += $$copyToDest($${OUT_PWD}/$${BUILDT}/$${TARGET}.dll, $$PROJ/app/$$BUILDT)
}

unix:!android:!macx:!ios {

QMAKE_POST_LINK += $$copyToDest($${OUT_PWD}/$$BUILDT/lib$${TARGET}.so, $$PROJ/app/$$BUILDT)

}
   
macx {
   QMAKE_POST_LINK += $$copyToDest($${OUT_PWD}/$$BUILDT/lib$${TARGET}.dylib, $$PROJ/app/$$BUILDT)

}

### //////////////////  ///////////////////////////////////////////

SOURCES = \
        emu.c \
        main.cpp \
        ifmagnetic.cpp \
        msg.cpp \
        emux.cpp \
        map.cpp \
        puzzle.cpp \
        puzman.cpp \
        save.cpp \
        xbr.cpp

HEADERS =  \
       defs.h \
       ifmagnetic.h \
       msg.h \
       emux.h \
       map.h \
       markup.h \
       diction.h \
       disasm.h \
       puzzle.h \
       save.h \
       sym.h \
       hunks.h \
       filters.h \
       pccp.h \
       puzpawn.h \
       puzguild.h \
    roomsguild.h \
    roomsjinxter.h \
    puzjinxter.h \
    roomsfish.h \
    puzfish.h

