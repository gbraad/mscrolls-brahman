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
#   Copyright (�) Voidware 2016-2017.
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
TARGET=png
CONFIG += static
QT -= core gui

PROJ = $$PWD/../../../..
include($$PROJ/app.pri)       

### ////////////////// BASE ///////////////////////////////////////////

unix:!android {
    #ZLIBDIR = $$PROJ/tools/zlib
    #LIBS += -L$$ZLIBDIR/$$LIBDIR -lz
}

INCLUDEPATH += $$ZLIBDIR


ios {
    QMAKE_SONAME_PREFIX = @rpath
}
    
### //////////////////  ///////////////////////////////////////////

SOURCES = \ 
    png.c \
    pngerror.c \
    pngget.c \
    pngmem.c \
    pngpread.c \
    pngread.c \
    pngrio.c \
    pngrtran.c \
    pngrutil.c \
    pngset.c \
    pngtrans.c \
    pngwio.c \
    pngwrite.c \
    pngwtran.c \
    pngwutil.c
    
HEADERS = \  
    png.h \
    pngconf.h \
    pngdebug.h \
    pnginfo.h \
    pnglibconf.h \
    pngpriv.h \
    pngstruct.h



