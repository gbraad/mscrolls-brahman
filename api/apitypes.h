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

#include <iostream>
#include <string>
#include <memory>

#include "defs.h"
#include "utils.h"

// forward
struct Control;

// make_unique not in C++11,  use our own version in the meantime...
template<typename T, typename ...Args>
std::unique_ptr<T> _make_unique( Args&& ...args )
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)... ));
}

struct DLLX APIType
{
    typedef std::string         string;

    struct Notifier
    {
        virtual void changed() {}
    };

    struct NotifierSender
    {
        typedef std::vector<Notifier*> Notifiers;

        Notifiers       _notifiers;

        void addNotifier(Notifier* n) { _notifiers.push_back(n); }
        void removeNotfier(Notifier* n) { erase(_notifiers, n); }

        void notify()
        {
            for (size_t i = 0; i < _notifiers.size(); ++i)
                _notifiers[i]->changed();
        }
    };
};

// base type for API objects
template<class B> struct API: public APIType
{
    typedef std::shared_ptr<B>       Ref;
    static Ref create(Control* c) { return std::make_shared<B>(c); }

    //typedef std::unique_ptr<B>       Ref;
    //static Ref create(Control* c) { return _make_unique<B>(c); }

    API(Control* c) : _control(c) {}

protected:

    Control*    _control;
};



