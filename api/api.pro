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


TEMPLATE=lib
TARGET=brahmanapi
QT =

PROJ = $$PWD/..
include($$PROJ/app.pri)

### ////////////////// BASE ///////////////////////////////////////////

INCLUDEPATH += $$RJSONDIR/include $$IFIDIR
DEFINES += IFI_BUILD IFI_IMPORT

win32 {
      LIBS += $$QMAKE_LIBS_GUI -liphlpapi
}

# windows and OSX need to link here, but linux etc link SO in app
LIBS += -L$$IFIDIR/$$LIBDIR -lifigame

win32 {

defineTest(copyToDestdir) {
    files = $$1

    for(FILE, files) {
        DDIR = $$PROJ/app/$$BUILDT

        # Replace slashes in paths with backslashes for Windows
        win32:FILE ~= s,/,\\,g
        win32:DDIR ~= s,/,\\,g

        QMAKE_POST_LINK += $$QMAKE_COPY $$shell_quote($$FILE) $$shell_quote($$DDIR) $$escape_expand(\\n\\t)
    }

    export(QMAKE_POST_LINK)
}

   copyToDestdir($${OUT_PWD}/$${BUILDT}/$${TARGET}.dll)
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

macx {

defineTest(copyToDestdir) {
    files = $$1

    for(FILE, files) {
        DDIR = $$PROJ/app/$$BUILDT
        QMAKE_POST_LINK += $$QMAKE_COPY $$shell_quote($$FILE) $$shell_quote($$DDIR) $$escape_expand(\\n\\t)
    }

    export(QMAKE_POST_LINK)
    }

   copyToDestdir($${OUT_PWD}/$${BUILDT}/lib$${TARGET}.dylib)
   copyToDestdir($${OUT_PWD}/$${BUILDT}/lib$${TARGET}.1.dylib)
   copyToDestdir($${OUT_PWD}/$${BUILDT}/lib$${TARGET}.1.0.dylib)
   copyToDestdir($${OUT_PWD}/$${BUILDT}/lib$${TARGET}.1.0.0.dylib)

    QMAKE_LFLAGS_SONAME  = -Wl,-install_name,@executable_path/../Frameworks/
}

SOURCES = \
  control.cpp \
  os.cpp \
  prefs.cpp \
  transcript.cpp \
  console.cpp \
  itemsmodel.cpp \
  mapbox.cpp 
  
win32 {
  SOURCES += windpi.cpp
  LIBS += -lsetupapi
}

HEADERS = \
  apitypes.h \
  control.h \
  imptypes.h \
  os.h \
  prefs.h \
  transcript.h \
  transcripti.h \
  textpage.h \
  console.h \
  consolei.h \
  itemsmodel.h \
  mapbox.h \
  mapboxi.h \
  ifengine.h \
  ifmap.h \
  ifinfo.h \
  ifroster.h  \
  wordstat.h

