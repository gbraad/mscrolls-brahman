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

# This project just builds stub and makes a stub ifigame.dll

TEMPLATE=lib
TARGET=ifigame
QT = 


PROJ = $$PWD/..
include($$PROJ/app.pri)       

### ////////////////// BASE ///////////////////////////////////////////

ios {
    CONFIG += static
}

INCLUDEPATH += $$IFIDIR
DEFINES += IFI_BUILD

macx {
    QMAKE_LFLAGS_SONAME  = -Wl,-install_name,@executable_path/../Frameworks/

    QMAKE_POST_LINK += $$copyToDest($${OUT_PWD}/$${BUILDT}/lib$${TARGET}.dylib, $$PROJ/app/$$BUILDT)
    QMAKE_POST_LINK += $$copyToDest($${OUT_PWD}/$${BUILDT}/lib$${TARGET}.1.dylib, $$PROJ/app/$$BUILDT)
    QMAKE_POST_LINK += $$copyToDest($${OUT_PWD}/$${BUILDT}/lib$${TARGET}.1.0.dylib, $$PROJ/app/$$BUILDT)
    QMAKE_POST_LINK += $$copyToDest($${OUT_PWD}/$${BUILDT}/lib$${TARGET}.1.0.0.dylib, $$PROJ/app/$$BUILDT)

    export(QMAKE_POST_LINK)                   
}

unix:!android:!macx:!ios {

defineTest(copyToDestdir) {
    files = $$1

    for(FILE, files) {
        DDIR = $$PROJ/app/$$BUILDT
        QMAKE_POST_LINK += $$QMAKE_COPY $$shell_quote($$FILE) $$shell_quote($$DDIR) $$escape_expand(\\n\\t)
    }

    export(QMAKE_POST_LINK)
}

   copyToDestdir($${OUT_PWD}/$${BUILDT}/lib$${TARGET}.so)
   copyToDestdir($${OUT_PWD}/$${BUILDT}/lib$${TARGET}.so.1)
   copyToDestdir($${OUT_PWD}/$${BUILDT}/lib$${TARGET}.so.1.0)
   copyToDestdir($${OUT_PWD}/$${BUILDT}/lib$${TARGET}.so.1.0.0)
}

win32 {
   
    QMAKE_POST_LINK += $$copyToDest($${OUT_PWD}/$${BUILDT}/$${TARGET}.dll, $$PROJ/app/$$BUILDT)

    export(QMAKE_POST_LINK)                   
}

### //////////////////  ///////////////////////////////////////////

SOURCES = \
stub.cpp

dummy {
# dummy game backend, build with qmake config+=dummy
SOURCES = dummygame.cpp ifiglue.cpp
}

target.path=$$PROJ/app/$$BUILDT
INSTALLS += target
