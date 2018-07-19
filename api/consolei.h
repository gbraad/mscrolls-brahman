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

#include "imptypes.h"
#include "control.h"
#include "console.h"
#include "textpage.h"
#include "logged.h"
#include "ifengine.h"

struct Console::Imp : public ImpType<Console>
{
    Imp(Console* host) : ImpType(host) { _init(); }

    void charEmit(char c)
    {
        if (c)
        {
            char buf[2];
            buf[0] = c;
            buf[1] = 0;
            _page.append(buf);
        }
        else
        {
            // signal segment complete
            _page.finish();
            
            // emit changed event
            if (_host->_notifier) _host->_notifier->changed();
        }
    }

    struct Completer
    {
        Completer() { _inQuote = false; _plevel = 0; }
        
        bool ok() const { return !_inQuote && _plevel == 0; }

        bool _inQuote;
        int  _plevel;
    };

    string lineAt(int pos) const
    {
        int l = _page.lineForPos(pos);
        if (l >= 0)
        {
            return _page.nthLine(l);
        }
        return string();
    }
    
    void _init() 
    {
        _be = 0;

        // 4k of console input history
        _page.maxSize(4096); 
    }

    // we're given a reference from the controller
    IFEngineInterface*           _be;
    TextPage                     _page;
    
};





