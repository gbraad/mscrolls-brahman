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
#   Copyright (©) Voidware 2017-2018.
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
TEMPLATE=app
TARGET=ificonsole
CONFIG += console
CONFIG -= app_bundle
QT = 

PROJ = $$PWD/../..
include($$PROJ/app.pri)       

### ////////////////// BASE ///////////////////////////////////////////

STRDIR = $$PROJ/strands
INCLUDEPATH += $$IFIDIR

# consider also IFI_STATIC
DEFINES += IFI_BUILD IFI_IMPORT IFI_HANDLE_CHOICE

win32:!msvc {
   QMAKE_POST_LINK += $$copyToDest($$STRDIR/$$LIBDIR/ifigame.dll, $${OUT_PWD}/$${BUILDT})
}

LIBS = -L$$STRDIR/$$LIBDIR -lifigame

### //////////////////  ///////////////////////////////////////////

SOURCES += $$IFIDIR/ificonsole.cpp

