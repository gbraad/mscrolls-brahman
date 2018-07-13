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

TEMPLATE=app
TARGET=xc
QT =

CONFIG += console
CONFIG -= app_bundle

PROJ = $$PWD/../..
include($$PROJ/app.pri)       

### //////////////////  ///////////////////////////////////////////

ENG = Languages/ENG
NL = Languages/NL

INCLUDEPATH += $$ENG $$NL

SOURCES = \
Init.c \
debug.c \
endian.c \
fileio.c \
keyword.c \
prsedict.c \
parsverb.c \
prsassgn.c \
prsdescr.c \
prsedsys.c \
prsefun.c \
prseloc.c \
prseobj.c \
prsesntc.c \
prsetims.c \
prsetrig.c \
prsstory.c \
storyinf.c \
tables.c \
errors.c \
fndctrig.c \
$$ENG/ENG-checksyntax.c \
$$ENG/ENG-checkwc.c \
$$ENG/ENG-keyword.c \
$$ENG/ENG-predefs.c \
$$NL/NL-checksyntax.c \
$$NL/NL-checkwc.c \
$$NL/NL-keyword.c \
$$NL/NL-predefs.c

HEADERS =  \ 
color.h \
debug.h \
fileio.h \
init.h \
keyword.h \
prsedict.h \
parsverb.h \
prsassgn.h \
prsdescr.h \
prsedsys.h \
prsefun.h \
prseloc.h \
prseobj.h \
prsesntc.h \
prsetims.h \
prsetrig.h \
prsstory.h \
storyinf.h \
tables.h \
typedefs.h  \
errors.h \
fndctrig.h \
$$ENG/ENG-checksyntax.h \
$$ENG/ENG-errors.h \
$$ENG/ENG-predefs.h \
$$NL/NL-checksyntax.h \
$$NL/NL-errors.h \
$$NL/NL-kwtable.h \
$$NL/NL-predefs.h
