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
 
struct RosterInfo: public InfoBase {};

struct RosterInfoBuilder: public InfoBuilder
{
    RosterInfoBuilder(emitFn* e, void* ctx) : InfoBuilder(e, ctx) {}

    void beginItems() { beginArray(BRA_INV_ITEMS); }
    void endItems() { endArray(); }

    void beginItem() { beginObj(); }
    void endItem() { end(); }

    void itemID(int id) { tagInt(BRA_INV_ID, id); }
    void itemName(const char* name) { tagString(BRA_INV_NAME, name); }
    void itemName(const string& name) { tagString(BRA_INV_NAME, name); }
    void itemWorn(bool v = true) { tagBool(BRA_INV_WORN, v); }

    void beginPeople() { beginArray(BRA_INV_PEOPLE); }
    void endPeople() { endArray(); }

    void beginPerson() { beginObj(); }
    void endPerson() { end(); }

    void personID(int id) { tagInt(BRA_INV_ID, id); }
    void personName(const char* name) { tagString(BRA_INV_NAME, name); }
    void personName(const string& name) { tagString(BRA_INV_NAME, name); }
    
};

