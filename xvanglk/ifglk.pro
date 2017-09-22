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
#   Copyright (©) Voidware 2016-2017.
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

# other templates: subdirs, vclib, vcapp, lib, app, aux
TEMPLATE=lib
TARGET=xvan
CONFIG += c++11 plugin
QT += core

#VERSION = 1.2.3

PROJ = $$PWD/..

### ////////////////// BASE ///////////////////////////////////////////

gcc:QMAKE_CXXFLAGS_WARN_ON = -Wall -Wno-unused-parameter -Wno-unknown-pragmas -Wno-unused-result -Wno-switch -Wno-parentheses -Wno-format
gcc:QMAKE_CFLAGS_WARN_ON = -Wall -Wno-unused-parameter -Wno-unknown-pragmas -Wno-unused-result -Wno-switch -Wno-parentheses -Wno-pointer-sign -Wno-unused-but-set-variable

INCLUDEPATH += $$PROJ/common $$PROJ/api $$PROJ/app
DEFINES += LOG_MT NO_ANIMATION

win32 {
   DEFINES += _WIN32_WINNT=0x600
   DEFINES -= UNICODE
}

CONFIG(debug, debug|release) {
   DEFINES += DEBUG_MODE LOGGING
   BUILDT=debug
}

CONFIG(release, debug|release) {
   DEFINES += NDEBUG QT_NO_DEBUG_OUTPUT
   BUILDT=release
}

unix:!android {
    DESTDIR=$$BUILDT
    OBJECTS_DIR = $$BUILDT
    MOC_DIR = $$BUILDT
}

win32 {
defineReplace(copyToDest) {
    files = $$1
    DDIR = $$2
    LINK=
    win32:DDIR ~= s,/,\\,g
    for(FILE, files) {
        # Replace slashes in paths with backslashes for Windows
        win32:FILE ~= s,/,\\,g
        LINK += $$QMAKE_COPY $$quote($$FILE) $$quote($$DDIR) $$escape_expand(\\n\\t)
    }
    return ($$LINK)
}

   QMAKE_POST_LINK += $$copyToDest($${OUT_PWD}/$${BUILDT}/$${TARGET}.dll, $$PROJ/app/$$BUILDT)
}

unix:!android {

defineReplace(copyToDest) {
    files = $$1
    DDIR = $$2
    LINK=
    for(FILE, files) {
        LINK += $$QMAKE_COPY $$quote($$FILE) $$quote($$DDIR) $$escape_expand(\\n\\t)
    }
    return ($$LINK)
}

   QMAKE_POST_LINK += $$copyToDest($${OUT_PWD}/$$BUILDT/lib$${TARGET}.so, $$PROJ/app/$$BUILDT)

}
   


### //////////////////  ///////////////////////////////////////////

SOURCES = \
ifglk.cpp \
arithmtc.c \
checkpar.c \
cleanup.c \
debug.c \
dirs.c \
disambig.c \
endian.c \
execute.c \
fileio.c \
init.c \
input.c \
letsplay.c \
output.c \
readfun.c \
readloc.c \
readobj.c \
readtrig.c \
readverb.c \
restore.c \
save.c \
spanTree.c \
syntax.c \
sysfunc.c \
timers.c \
trnslate.c \
xeqfun.c 

HEADERS =  \
ifglk.h \
glk.h \
arithmtc.h \
checkpar.h \
cleanup.h \
debug.h \
dirs.h \
disambig.h \
execute.h \
fileio.h \
init.h \
input.h \
keyword.h \
letsplay.h \
output.h \
readfun.h \
readloc.h \
readobj.h \
readtrig.h \
readverb.h \
restore.h \
save.h \
spantree.h \
syntax.h \
sysfunc.h \
timers.h \
trnslate.h \
typedefs.h \
xeqfun.h 

