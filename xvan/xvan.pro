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
TARGET=ifigame
QT = 

PROJ = $$PWD/..
include($$PROJ/app.pri)       

### ////////////////// BASE ///////////////////////////////////////////

INCLUDEPATH += $$IFIDIR
DEFINES += IFI_BUILD

### //////////////////  ///////////////////////////////////////////

SOURCES = \
arithmtc.c checkpar.c cleanup.c debug.c dirs.c disambig.c endian.c execute.c fileio.c init.c input.c letsplay.c lets-try.c output.c readfun.c readloc.c readobj.c readtrig.c readverb.c restore.c save.c spanTree.c syntax.c sysfunc.c timers.c trnslate.c xeqfun.c errors.c ENG-checksyntax.c ENG-moreinfo.c ENG-xeqfun.c ENG-article.c NL-checksyntax.c NL-moreinfo.c NL-xeqfun.c NL-article.c json.c IFI.c play.c jsonbld.c descr2str.c base64.c \
ifixvan.cpp

### //////////////////  ///////////////////////////////////////////

include($$IFIDIR/ificlient.pri)

app {
# build stand alone console app
# type, qmake CONFIG+=app

TEMPLATE=app
CONFIG += console
CONFIG -= dll
DEFINES -= IFI_BUILD
SOURCES -= $$IFIDIR/ifiglue.cpp
SOURCES -= ifixvan.cpp
}
