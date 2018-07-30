/*
 *
 *
 *   ______              _
 *   | ___ \            | |
 *   | |_/ / _ __  __ _ | |__   _ __ ___    __ _  _ __
 *   | ___ \| '__|/ _` || '_ \ | '_ ` _ \  / _` || '_ \
 *   | |_/ /| |  | (_| || | | || | | | | || (_| || | | |
 *   \____/ |_|   \__,_||_| |_||_| |_| |_| \__,_||_| |_|
 *
 *
 *  "The creative principle which lies realized in the whole world"
 *
 *  Copyright (c) Strand Games 2018.
 *
 *  This program is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU Lesser General Public License (LGPL) as published
 *  by the Free Software Foundation, either version 3 of the License, or (at
 *  your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
 *  for more details.
 * 
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 *  contact@strandgames.com
 *
 */


#pragma once

#define IFI_COMMAND     "command"
#define IFI_CONFIGDIR   "configdir"
#define IFI_DATADIR     "datadir"
#define IFI_STORY       "story"
#define IFI_OBJECTS     "objects"

#define IFI_LOADDATA    "loaddata"
#define IFI_SAVEDATA    "savedata"
#define IFI_MAP         "map"
#define IFI_PICTURE     "picture"
#define IFI_SOUND       "sound"
#define IFI_ITEMS       "items"
#define IFI_PEOPLE      "people"
#define IFI_META        "meta"
#define IFI_MOVES       "moves"
#define IFI_VALUES      "values"
#define IFI_RANDOMSEED  "randomseed"
#define IFI_DATA        "data"
#define IFI_BEGIN       "begin"

// reply only

#define IFI_TEXT        "text"
#define IFI_TITLE       "title"
#define IFI_EXITS       "exits"
#define IFI_PROMPT      "prompt"
#define IFI_NAME        "name"
#define IFI_BRIGHTNESS  "brightness"
#define IFI_CONTRAST    "contrast"
#define IFI_SATURATION  "saturation"
#define IFI_LIGHTNESS   "lightness"
#define IFI_GAMMA       "gamma"
#define IFI_ID          "id"
#define IFI_WORN        "worn"
#define IFI_ICON        "icon"
#define IFI_PLACES      "places"
#define IFI_BACKIMAGE   "backimage"
#define IFI_GX          "gx"
#define IFI_GY          "gy"
#define IFI_DARK        "dark"
#define IFI_SHOWUP      "up"
#define IFI_SHOWDOWN    "down"
#define IFI_LOCATION    "location"
#define IFI_UNUSE       "unuse"
#define IFI_USEWITH     "usewith"
#define IFI_USE         "use"
#define IFI_COMPASSGO   "compassgo"
#define IFI_REFRESHCMD "refreshcmd"

// metaobj
//#define IFI_TITLE       "title"
#define IFI_AUTHOR      "author"
#define IFI_ORGANISATION "organisation"
#define IFI_CREDITS     "credits"
#define IFI_COVERTEXT   "covertext"
#define IFI_VERSION     "version"
#define IFI_ANDROID_MARKET "android_market"
#define IFI_IOS_MARKET  "ios_market"
#define IFI_EFFECT      "effect"
#define IFI_UI_SIDEBAR  "ui_sidebar"
#define IFI_UI_TEXTINPUT "ui_textinput"
#define IFI_UI_COMPASS  "ui_compass"
//#define IFI_BACKIMAGE   "backimage"
#define IFI_PRIME_COL   "primary_color"
#define IFI_CONTRAST_COL "contrast_color"
#define IFI_AUTOLINK    "autolink"
#define IFI_UI_RESTART  "ui_restart"
//#define IFI_TITLE       "title"

// textobj
//#define IFI_TEXT      "text"
//#define IFI_ID        "id"
#define IFI_FONT       "font"
#define IFI_FONTWEIGHT  "fontweight"
#define IFI_COLOR       "color"

// soundobj
//#define IFI_NAME        "name"
#define IFI_DURATION    "duration"

// masks for directions
#define IFI_DIR_N       1
#define IFI_DIR_NE      2
#define IFI_DIR_E       4
#define IFI_DIR_SE      8
#define IFI_DIR_S       16
#define IFI_DIR_SW      32
#define IFI_DIR_W       64
#define IFI_DIR_NW      128
#define IFI_DIR_UP      256
#define IFI_DIR_DOWN    512
#define IFI_DIR_IN      1024
#define IFI_DIR_OUT     2048

///// Eval Special Commands

// drop from menu
#define IFI_UNUSE_DEFAULT "unuse {1:name}"

// for use X with Y
#define IFI_USEWITH_DEFAULT "use {1:name} with {2:name}"

// click on sidebar
#define IFI_USE_DEFAULT "use {1:name}"

#define IFI_COMPASSGO_DEFAULT "{1:id}"

#define IFI_REFRESHCMD_DEFAULT "look"


