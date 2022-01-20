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


#include "transcripti.h"

void Transcript::_init()
{
    _imp = new Imp(this);
    
    _inlineImages = false;
}

Transcript::~Transcript() { delete _imp; }

typedef std::string string;

string Transcript::text() const { return _imp->_page.toString(); }
string Transcript::textHTML() const { return _imp->textHTML(); }
string Transcript::customJSON() const { return _imp->_customJSON; }
void Transcript::resetCustomJSON() { _imp->_customJSON.clear(); }
void Transcript::clear() { _imp->clear(); }

int Transcript:: width() const { return _imp->_page._width; }
void Transcript::setWidth(int w) { _imp->_page._width = w; }
string Transcript::styleHTML() const { return _imp->styleHTML(); }
bool Transcript::notifyPending() { return _imp->notifyPending(); }



