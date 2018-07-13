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

#include <string>
#include <functional>
#include <assert.h>
#include "control.h"

struct Updater
{
    typedef std::string string;

    struct UpdateInfo
    {
        UpdateInfo()
        {
            _ok = false;
            _updateAvailable = false;
        }
        
        bool    _updateAvailable;
        bool    _ok; // or msg is error
        string  _msg;
    };

    typedef std::function<void()> UpdateHandler;
    UpdateInfo _info;
    
    Updater(Control* c) : _control(c) 
    {
        assert(_control);
        _currentVersion = _control->currentVersion();
    }

    bool isVersionNewer(const char* v)
    {
        // compare given version string `v` to existing.
        // ASSUME both strings are X.Y.Z

        bool res = false;

        std::vector<string> oldV;
        split(oldV, _currentVersion, '.');

        std::vector<string> newV;
        split(newV, v, '.');

        for (size_t i = 0; i < newV.size(); ++i)
        {
            if (i >= oldV.size())
            {
                // eg 1.2.3 vs 1.2
                res = true;
                break;
            }

            int a = atoi(newV[i].c_str());
            int b = atoi(oldV[i].c_str());

            if (a < b) break; // older
            
            if (a > b)
            {
                // newer
                res = true;
                break;
            }

            // same
        }

        LOG3("compare versions ", v << " > " << _currentVersion << " = " << res);
        return res;
    }

    virtual ~Updater() {}

    virtual void start() {}
    virtual bool check(UpdateHandler r) { return false; }

private:

    Control*    _control;
    string      _currentVersion;
};


