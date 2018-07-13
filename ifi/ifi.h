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

#ifdef _WIN32
#define DLLEXPORT __declspec(dllexport)
#define DLLIMPORT __declspec(dllimport)

#ifdef IFI_STATIC
#define DLL
#else

#ifdef IFI_IMPORT
#define DLL DLLIMPORT
#else
#define DLL DLLEXPORT
#endif

#endif // IFI_STATIC
#else
#define DLL
#endif // _WIN32

struct DLL IFI
{
    typedef void charEmitFn(void*, const char*);

    virtual ~IFI() {}

    static IFI* create();
    
    virtual void setEmitter(charEmitFn* emitter, void* ctx) = 0;
    virtual bool eval(const char* json) = 0;
    virtual bool start(int argc, char** argv) = 0;

    // return -1 if cannot sync
    // return 0 if timeout yield
    // return 1 if sync ok
    virtual int sync(int timeoutms = 0) = 0;
    
    virtual void release() = 0;

};

