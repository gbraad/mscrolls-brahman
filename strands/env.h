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
 *  Copyright (c) Strand Games 2020.
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

#include "list.h"

struct EnvArgs
{
    EnvArgs(int n) : _args(n) {}

    // args will be raw list of args during function eval
    List            _args;

    // if we need to convert args to term, args will be empty and
    // argsTerm will be valid
    Term            _argsTerm;
    EnvArgs*        _prev;
};
    
struct Env
{
    Env() { _args = 0; }
    Env(const Term& e, EnvArgs* a) : _env(e), _args(a) {}
        
    Term            _env;
    EnvArgs*        _args;

    void push(EnvArgs* a)
    {
        a->_prev = _args;
        _args = a;
    }

    void pop()
    {
        assert(_args);
        _args = _args->_prev;
    }
};
