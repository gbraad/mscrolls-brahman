# other templates: subdirs, vclib, vcapp, lib, app, aux
TEMPLATE=app
TARGET=xc
QT += core
QT -= gui

CONFIG += console
CONFIG -= app_bundle

PROJ = $$PWD

### //////////////////  ///////////////////////////////////////////

SOURCES = \
Init.c \
debug.c \
endian.c \
fileio.c \
keyword.c \
output.c \
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
tables.c 

HEADERS =  \ 
color.h \
debug.h \
fileio.h \
init.h \
keyword.h \
output.h \
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
typedefs.h 
