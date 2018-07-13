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
#include "ifmap.h"
#include "ifroster.h"

// forward
struct VarSet;
struct ProductInfo;

class IFEngineInterface
{
public:

    typedef std::string string;
    typedef void charEmitFn(void*, char);
    typedef void segmentInfoFn(void*, char);

    struct CommandResultI
    {
        enum Operation
            {
                op_normal = 0,
                op_dobj_names = 1,
                op_dobj_description = 2,
                op_capture = 3,
            };

        Operation   _op;

        CommandResultI()
        {
            _op = op_dobj_names;
        }

        virtual ~CommandResultI() {}
        virtual const char* toString() const = 0;
    };

    virtual ~IFEngineInterface() {}

    virtual bool start(const char* configDir,
                       const char* dataDir,
                       const char* story,
                       charEmitFn* transcriptEmit, void* tctx,
                       charEmitFn* consoleEmit, void* cctx,
                       segmentInfoFn* sfn, void* sctx) = 0;
    virtual void emitScene() = 0;
    virtual void promptConsole() = 0;
    virtual CommandResultI* makeResult() = 0;
    virtual bool evalCommand(const char* cmd,
                             CommandResultI* res = 0,
                             bool echo = false) = 0;
    
    virtual void clearImage() = 0;
    virtual bool evalUseXwithY(const string& x, const string& y,
                               CommandResultI* res = 0) = 0;
    virtual bool updateMapInfo(MapInfo&) { return false; }
    virtual bool updateRosterInfo(RosterInfo&) { return false; }
    virtual bool getProductInfo(ProductInfo&) { return false; }
    virtual string currentVersion() const { return "1"; } // default

    // options is now used to pass various things to the engine
    // a `VarSet` is used (rather than, say JSON) to pass in, because
    // it's simpler and does not create a dependency on the backend for 
    // a JSON lib. 
    //
    // see braschema.h for supported option tags (eg "loglevel")
    virtual bool setOptions(const VarSet&) { return false; }

    virtual bool saveGame(const char* name) { return false; }
    virtual bool loadGame(const char* name) { return false; }
    virtual bool restartGame() { return false; }


    typedef bool RequestExtraFn(void* ctx, const unsigned char** data,
                                size_t* amt);

    virtual bool setRequestSaveExtra(RequestExtraFn* fn, void* ctx)
    { return false; }
    
    virtual bool setRequestLoadExtra(RequestExtraFn* fn, void* ctx)
    { return false; }
    
};

#define IFENGINE_IID "com.voidware.brahman.ifengine"

#define DECLARE_IFENGINE \
  Q_DECLARE_INTERFACE(IFEngineInterface, IFENGINE_IID)


