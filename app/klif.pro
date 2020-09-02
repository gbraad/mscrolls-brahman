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

TARGET = "klif"
FTARGET = "klif.app"

DEFINES += THE_GAME="klif.h"

include(app.pro)       

macx: !android {
        data.path = "$$PROJ/app/$$BUILDT/$$FTARGET/Contents/Resources/assets"
        INSTALLS += data
        #ICON = $$PROJ/app/icons-story/story.icns
}

unix: !android: !macx {
        data.path = "$$PROJ/app/$$BUILDT/assets"
        INSTALLS += data
}

ios {
    data.path = assets
    QMAKE_BUNDLE_DATA += data

    app_launch_images.files = $$PWD/ios-story/LaunchScreen.storyboard
    QMAKE_BUNDLE_DATA += app_launch_images
    QMAKE_INFO_PLIST = ios-story/Info.plist

    QMAKE_ASSET_CATALOGS += ios-story/Assets.xcassets
}

win32 {
   RC_ICONS = icons/brahman_logo.ico
}

HEADERS += klif.h

# icons
#RESOURCES += story.qrc

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android-story

DISTFILES += \
ios-story/LaunchScreen.storyboard

include(../spine/spineqml/spineqml.pri)

    
        
