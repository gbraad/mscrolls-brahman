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

#include "apitypes.h"

struct DLLX Transcript: public API<Transcript>
{
    Transcript(Control* c) : API(c), _notifier(0) { _init(); }
    ~Transcript();
    
    struct Notifier
    {
        virtual void changed(int what = 0) {}
    };


    Notifier*           _notifier;
    bool                _inlineImages;
    unsigned int        _linkColor = 0; // not set
    unsigned int        _consoleEchoColor = 0; // not set
    bool                _refreshOnNewline = false;

    void _init();

    string              text() const;
    string              textHTML() const;
    string              customJSON() const;
    void                resetCustomJSON();

    string              styleHTML() const;

    int                 width() const;
    void                setWidth(int w);

    
    struct Imp;
    Imp*                _imp;
};

