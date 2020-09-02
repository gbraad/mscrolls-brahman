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

#define PERREND ", at '" << _snippet() << "' " << _filename << ':' << lineno << std::endl

#define PERR0(_m) std::cout << _m << PERREND
#define PERR1(_m, _a) std::cout << _m << ' ' << _a << PERREND

#define PERR2(_m, _a, _b) std::cout << _m << ' ' << _a << ' ' << _b << PERREND
#define PERR3(_m, _a, _b, _c) std::cout << _m << ' ' << _a << ' ' << _b << ' ' << _c << PERREND
#define PERR4(_m, _a, _b, _c, _d) std::cout << _m << ' ' << _a << ' ' << _b << ' ' << _c << ' ' << _d << PERREND

#define PW0(_m) PERR0(_m)
#define PW1(_m, _a) PERR1(_m, _a)

#define ERREND std::endl

#define ERR0(_m) std::cout << _m << ERREND
#define ERR1(_m, _a) std::cout << _m << ' ' << _a << ERREND

#define ERR2(_m, _a, _b) std::cout << _m << ' ' << _a << ' ' << _b << ERREND
#define ERR3(_m, _a, _b, _c) std::cout << _m << ' ' << _a << ' ' << _b << ' ' << _c << ERREND
#define ERR4(_m, _a, _b, _c, _d) std::cout << _m << ' ' << _a << ' ' << _b << ' ' << _c << ' ' << _d << ERREND

#define W0(_m) ERR0(_m)
#define W1(_m, _a) ERR1(_m, _a)

#define DLOG0(_cc, _m) if (_cc) ERR0(_m)
#define DLOG1(_cc, _m, _a) if (_cc) ERR1(_m, _a)
#define DLOG2(_cc, _m, _a, _b) if (_cc) ERR2(_m, _a, _b)
#define DLOG3(_cc, _m, _a, _b, _c) if (_cc) ERR3(_m, _a, _b, _c)
#define DLOG4(_cc, _m, _a, _b, _c, _d) if (_cc) ERR4(_m, _a, _b, _c, _d)


