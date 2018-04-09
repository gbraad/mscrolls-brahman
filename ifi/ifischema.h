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
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 * 
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 * 
 *  THE SOFTWARE IS PROVIDED "AS IS," WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 * 
 *  contact@strandgames.com
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
#define IFI_ITEMS       "items"
#define IFI_PEOPLE      "people"
#define IFI_META        "meta"
#define IFI_MOVES       "moves"
#define IFI_VALUES      "values"
#define IFI_RANDOMSEED  "randomseed"

// reply only

#define IFI_TEXT        "text"
#define IFI_TITLE       "title"
#define IFI_SCORE       "score"
#define IFI_SCOREMAX    "scoremax"
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

// textobj
//#define IFI_TEXT      "text"
#define IFI_FONT        "font"
#define IFI_FONTWEIGHT  "fontweight"
#define IFI_COLOR       "color"

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


