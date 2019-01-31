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

CONFIG += c++11

gcc:QMAKE_CXXFLAGS_WARN_ON = -Wall -Wno-unknown-pragmas -Wno-switch -Wno-unused-parameter
gcc:QMAKE_CFLAGS_WARN_ON = -Wall -Wno-unknown-pragmas -Wno-switch

QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.8

LIBPNG = $$PROJ/tools/apng/libpng_apng/libpng
LIBZ = $$PROJ/tools/zlib
RJSONDIR = $$PROJ/rapidjson
IFIDIR = $$PROJ/ifi
KLIFDIR = $$PROJ/klif

INCLUDEPATH += $$PROJ/common
DEFINES += LOG_MT

win32 {
   DEFINES += _WIN32_WINNT=0x600
   DEFINES -= UNICODE _UNICODE
}

CONFIG(debug, debug|release) {
   CONFIG += console
   DEFINES += LOGGING TIMERS
   BUILDT=debug
}

CONFIG(release, debug|release) {
   DEFINES += NDEBUG QT_NO_DEBUG_OUTPUT QT_NO_WARNING_OUTPUT 
   BUILDT=release
   }

linux: {
    LIBDIR = $$BUILDT
}
   
unix:!android {
    DESTDIR=$$BUILDT
    OBJECTS_DIR = $$BUILDT
    MOC_DIR = $$BUILDT
    RCC_DIR = $$BUILDT
}

win32 {
   LIBDIR = $$BUILDT
}

ios {
   LIBDIR = build-ios/$$BUILDT
   QMAKE_SONAME_PREFIX = @rpath
}

macx {
   LIBDIR = $$BUILDT
}

android {
    equals(ANDROID_TARGET_ARCH, armeabi-v7a) { 
        LIBDIR = build-android-arm-$$BUILDT
    }
    equals(ANDROID_TARGET_ARCH, armeabi) {
        LIBDIR = build-android-arm6-$$BUILDT
    }
    equals(ANDROID_TARGET_ARCH, x86)  {
       LIBDIR = build-android-x86-$$BUILDT
    }
}

win32:!msvc {

defineReplace(copyToDest) {
    files = $$1
    DDIR = $$2
    LINK=
    win32:DDIR ~= s,/,\\,g
    for(FILE, files) {
        # Replace slashes in paths with backslashes for Windows
        win32:FILE ~= s,/,\\,g
        LINK += $$QMAKE_COPY $$shell_quote($$FILE) $$shell_quote($$DDIR) $$escape_expand(\\n\\t)
    }
    return ($$LINK)
}
}

unix:!android:!macx:!ios {

defineReplace(copyToDest) {
    files = $$1
    DDIR = $$2
    LINK=
    for(FILE, files) {
        LINK += $$QMAKE_COPY $$shell_quote($$FILE) $$shell_quote($$DDIR) $$escape_expand(\\n\\t)
    }
    return ($$LINK)
}
}


macx {

defineReplace(copyToDest) {
    files = $$1
    DDIR = $$2
    LINK=
    for(FILE, files) {
        LINK += $$QMAKE_COPY $$shell_quote($$FILE) $$shell_quote($$DDIR) $$escape_expand(\\n\\t)
    }
    return ($$LINK)
}
}
