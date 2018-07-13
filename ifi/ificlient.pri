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



PROJ = $$PWD/..
IFIDIR = $$PROJ/ifi

win32:!msvc {

   DDIR = $$PROJ/app/$$BUILDT
   QMAKE_POST_LINK += $$copyToDest($${OUT_PWD}/$${BUILDT}/$${TARGET}.dll, $$DDIR)
}

unix:!android:!macx:!ios {

defineTest(copyToDestdir) {
    file = $$1
    DDIR = $$PROJ/app/$$BUILDT
    QMAKE_POST_LINK += $$copyToDest($$file, $$DDIR)
    export(QMAKE_POST_LINK)
}

   copyToDestdir($${OUT_PWD}/$${BUILDT}/lib$${TARGET}.so)
   copyToDestdir($${OUT_PWD}/$${BUILDT}/lib$${TARGET}.so.1)
   copyToDestdir($${OUT_PWD}/$${BUILDT}/lib$${TARGET}.so.1.0)
   copyToDestdir($${OUT_PWD}/$${BUILDT}/lib$${TARGET}.so.1.0.0)
}

macx {

defineTest(copyToDestdir) {
    file = $$1
    DDIR = $$PROJ/app/$$BUILDT
    QMAKE_POST_LINK += $$copyToDest($$file, $$DDIR)
    export(QMAKE_POST_LINK)
}

   copyToDestdir($${OUT_PWD}/$${BUILDT}/lib$${TARGET}.dylib)
   copyToDestdir($${OUT_PWD}/$${BUILDT}/lib$${TARGET}.1.dylib)
   copyToDestdir($${OUT_PWD}/$${BUILDT}/lib$${TARGET}.1.0.dylib)
   copyToDestdir($${OUT_PWD}/$${BUILDT}/lib$${TARGET}.1.0.0.dylib)

   QMAKE_LFLAGS_SONAME  = -Wl,-install_name,@executable_path/../Frameworks/
}


SOURCES += $$IFIDIR/ifiglue.cpp

XXXHEADERS +=  \
$$IFIDIR/ifi.h \
$$IFIDIR/ificlient.h \
$$IFIDIR/ifiglue.h \
$$IFIDIR/ifihandler.h \
$$IFIDIR/ifischema.h \
$$IFIDIR/worker.h 

