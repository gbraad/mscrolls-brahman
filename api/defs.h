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

#define DEFAULT_DATA_PATH "."
#define DEFAULT_DATA_DIRNAME "brahman"
#define PREFS_FILENAME "prefs.jsn"

#define PREFS_UPDATE_ENABLED "update_enabled"
#define DEFAULT_UPDATE_ENABLE 1

#define PREFS_MUSIC_ENABLED "music_enabled"
#define DEFAULT_MUSIC_ENABLE 1

#define PREFS_MODERN_ENABLED "modern_enabled"
#define DEFAULT_MODERN_ENABLE 1

#define PREFS_DP_SCALE  "dp_scale"
#define DEFAULT_DP_SCALE 1

#define PREFS_GAME_FONT  "game_font"
#define DEFAULT_GAME_FONT ""

#define PREFS_MAPBOXSIZE  "mapbox_size"
#define DEFAULT_MAPBOXSIZE 10

#define PREFS_TUTORIAL_LEVEL  "tutorial_level"
#define DEFAULT_TUTORIAL_LEVEL 0

#define PREFS_IMAGEADJ_ENABLED  "image_adjust_enabled"
#define DEFAULT_IMAGEADJ_ENABLE 1

#define PREFS_IMAGEPIXSCALE_ENABLED  "image_pixscale_enabled"
#define DEFAULT_IMAGEPIXSCALE_ENABLE 1

#define PREFS_IMAGESCALE_ENABLED  "image_scale_enabled"
#define DEFAULT_IMAGESCALE_ENABLE 1

#define PREFS_COMPASSMOVE_ENABLED  "compass_move_enabled"
#define DEFAULT_COMPASSMOVE_ENABLE 1

#define PREFS_GAME_BACKGROUND_COLOR "game_background_col"
#define DEFAULT_GAME_BACKGROUND_COLOR ""

#define PREFS_GAME_PRIMARY_COLOR "game_primary_col"
#define DEFAULT_GAME_PRIMARY_COLOR ""

#define PREFS_GAME_CONTRAST_COLOR "game_contrast_col"
#define DEFAULT_GAME_CONTRAST_COLOR ""

#define PREFS_CONSOLEECHO_ENABLED "consoleecho_enabled"
#define DEFAULT_CONSOLEECHO_ENABLE 0

#ifdef _WIN32
#define DLLX   __declspec( dllexport )
#else
#define DLLX
#endif

#ifdef _MSC_VER
// disable some annoying warnings
#pragma warning(disable: 4251)
#pragma warning(disable: 4100)
#pragma warning(disable: 4244)
#endif

