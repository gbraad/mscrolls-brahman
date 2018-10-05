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

#include "ifinfo.h"

struct ProductInfo: public InfoBase {};

#define _PS(_name, _tag) \
void product ## _name(const char* name) { tagString(_tag, name); } \
void product ## _name(const string& name) { tagString(_tag, name); }

struct ProductInfoBuilder: public InfoBuilder
{
    ProductInfoBuilder(emitFn* e, void* ctx) : InfoBuilder(e, ctx) {}

    void beginProduct() { begin(); }
    void endProduct() { end(); }

    // info
    _PS(Title, BRA_PRO_TITLE);
    _PS(Author, BRA_PRO_AUTHOR);
    _PS(Credits, BRA_PRO_CREDITS);
    _PS(CoverText, BRA_PRO_COVERTEXT);
    _PS(CoverTextColor, BRA_PRO_COVERTEXT_COL);
    _PS(CoverEffect, BRA_PRO_COVER_EFFECT);
    _PS(CoverTextFont, BRA_PRO_COVERTEXT_FNT);
    _PS(CoverImage, BRA_PRO_COVER_IMAGE);
    _PS(MarketAndroid, BRA_PRO_MARKET_ANDROID);
    _PS(MarketIOS, BRA_PRO_MARKET_IOS);
    _PS(PrivacyPolicy, BRA_PRO_PRIVACY);

    void productCoverTextWeight(int w) { tagInt(BRA_PRO_COVERTEXT_W, w); }

    void productUISidebar(bool v) { tagBool(BRA_PRO_UI_SIDEBAR, v); }
    void productUITextbox(bool v) { tagBool(BRA_PRO_UI_TEXTBOX, v); }
    void productUICompass(bool v) { tagBool(BRA_PRO_UI_COMPASS, v); }

    _PS(ThemePrimeColor, BRA_PRO_THM_PRIME_COL);
    _PS(ThemeAccentColor, BRA_PRO_THM_ACCENT_COL);
    _PS(ThemeTabColor, BRA_PRO_THM_TAB_COL);
    _PS(ThemeContrastColor, BRA_PRO_THM_CONTR_COL);
    _PS(ThemeFont, BRA_PRO_THM_FONT);
    
    void productUISaveLoad(bool v) { tagBool(BRA_PRO_UI_SAVELOAD, v); }
    void productUIUpdate(bool v) { tagBool(BRA_PRO_UI_UPDATE, v); }
    void productUIClassic(bool v) { tagBool(BRA_PRO_UI_CLASSIC, v); }
    void productUIUndoRedo(bool v) { tagBool(BRA_PRO_UI_UNDOREDO, v); }

};

#undef _PS

