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

#define BRA_SEGMENT_NULL        1
#define BRA_SEGMENT_TITLE       2
#define BRA_SEGMENT_IMAGE       3
#define BRA_SEGMENT_JSON        4
#define BRA_SEGMENT_CLEAR       5

// obsolete
#define BRA_SEGMENT_ACTION      1
#define BRA_SEGMENT_ITEMS       1
#define BRA_SEGMENT_SCENE       0

// map JSON tags
#define BRA_MAP_PLACES          "places"
#define BRA_MAP_ID              "id"
#define BRA_MAP_NAME            "name"
#define BRA_MAP_GRIDX           "gx"
#define BRA_MAP_GRIDY           "gy"
#define BRA_MAP_EXITS           "exits"
#define BRA_MAP_BACKIMAGE       "backimage"
#define BRA_MAP_DARK            "dark"
#define BRA_MAP_IND_UP          "up"
#define BRA_MAP_IND_DOWN        "down"

// number of items dropped here.
#define BRA_MAP_ITEMCOUNT       "itemcount"


// roster JSON tags
#define BRA_INV_ITEMS           "items"
#define BRA_INV_PEOPLE          "people"
#define BRA_INV_NAME            "name"
#define BRA_INV_ID              "id"
#define BRA_INV_WORN            "worn"

// chooser tags
#define BRA_CHO_CHOICES         "choices"
#define BRA_CHO_TEXT            "text"
#define BRA_CHO_LABEL           "label"
#define BRA_CHO_ACTION          "action"
#define BRA_CHO_DISABLED        "disabled"
#define BRA_CHO_PAGE            "page"

// product & theme tags
#define BRA_PRO_TITLE           "title"
#define BRA_PRO_AUTHOR          "author"
#define BRA_PRO_CREDITS         "credits"
#define BRA_PRO_MARKET_ANDROID  "android_market"
#define BRA_PRO_MARKET_IOS      "ios_market"
#define BRA_PRO_COVERTEXT       "cover"
#define BRA_PRO_COVERTEXT_W     "covertext_weight"
#define BRA_PRO_COVERTEXT_FNT   "covertext_font"
#define BRA_PRO_COVERTEXT_COL   "covertext_col"
#define BRA_PRO_COVER_EFFECT    "cover_effect"
#define BRA_PRO_COVER_IMAGE     "cover_image"

#define BRA_PRO_THM_PRIME_COL   "prime_color"
#define BRA_PRO_THM_ACCENT_COL  "acc_color"
#define BRA_PRO_THM_TAB_COL     "tab_color"
#define BRA_PRO_THM_CONTR_COL   "con_color"
#define BRA_PRO_THM_FONT        "font"

// product UI tags
#define BRA_PRO_UI_SIDEBAR      "ui_sidebar"
#define BRA_PRO_UI_TEXTBOX      "ui_textbox"
#define BRA_PRO_UI_COMPASS      "ui_compass"

#define BRA_PRO_UI_SAVELOAD     "ui_saveload"
#define BRA_PRO_UI_UPDATE       "ui_update"
#define BRA_PRO_UI_CLASSIC      "ui_classic"
#define BRA_PRO_UI_UNDOREDO     "ui_undo"

// -----------------------------------------------------------------------
// picture

// type:string
// description: full path to file name.
#define BRA_PIC_NAME            "name"

// type: number, default: 0
#define BRA_PIC_BRIGHTNESS      "brightness"

// type: number, default: 0
#define BRA_PIC_CONTRAST        "contrast"

// type: number, default: 0
#define BRA_PIC_SATURATION      "saturation"

// type: number, default: 0
#define BRA_PIC_LIGHTNESS       "lightness"

// type: number, default: 1
#define BRA_PIC_GAMMA           "gamma"

// display within text
#define BRA_PIC_INLINE          "inline"

// -----------------------------------------------------------------------
// option

#define BRA_OPT_LOGLEVEL        "loglevel"
#define BRA_OPT_MODERN          "modern"
#define BRA_OPT_RANDOMSEED      "randomseed"
#define BRA_OPT_PIXELSCALE      "pixelscale"


#define BRA_WORDS_WORDS         "words"

#define BRA_STORY_JSON_ITEMS    "items"
#define BRA_STORY_JSON_ICON     "icon"


//----- HTML STYLES

#define BRA_CONSOLE_ECHO_STYLE  "console"


