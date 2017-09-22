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
 *  Copyright (©) Voidware 2016-2017.
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
 *  contact@voidware.com
 *
 */

#pragma once

// magnetic engine


#include <QObject>
#include <thread>
#include <chrono>
#include <mutex>
#include <assert.h>
#include <condition_variable>

#include "ifengine.h"
#include "logged.h"
#include "fd.h"
#include "braschema.h"
#include "puzzle.h"
#include "msg.h"
#include "save.h"
#include "timer.h"
#include "growbuf.h"

DECLARE_IFENGINE

class IFMagnetic : public QObject, public IFEngineInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID IFENGINE_IID);
    Q_INTERFACES(IFEngineInterface);

    typedef std::thread thread;
    typedef std::mutex mutex;
    typedef std::condition_variable condition_variable;

    struct CommandResult: public CommandResultI
    {
        string  _result;
        const char* toString() const override { return _result.c_str(); }
    };
    
    thread              _msTask;
    bool                _running;
    bool                _shutdown;
    bool                _starting;

    bool                _createImageDir;
    bool                _addCommandNewline;

    string              _story;

    // location of game files
    string              _configDir;

    // location of user data
    string              _dataDir;

    // location of images
    string              _imageDir;
    
    charEmitFn*         _transcriptEmit;
    void*               _tctx;
    charEmitFn*         _consoleEmit;
    void*               _cctx;
    segmentInfoFn*      _segmentInfoEmit;
    void*               _sictx;

    mutex               _syncLock;
    condition_variable  _waiter;
    bool                _waiting;
    bool                _inputReady;
    
    // ms task data
    int                 _ms_gfx;
    char                _outbuf[4096];
    size_t              _outpos;

    char                _inbuf[1024];
    size_t              _inpos;
    char*               _imageJSONBuf;

    char                _statusBuf[256];
    size_t              _statusPos;
    bool                _statusReady;

    uchar*              _gameSaveMemory;
    size_t              _gameSaveMemorySize;
    UndoState           _undos;

    PuzzleManager       _puzzles;
    bool                _puzzlesEnabled;  // modern mode
    Messages            _messages;
    string              _originalCredits;

    bool                _useXBR4;
    bool                _ignoreOutput;

public:

    struct PicRequest
    {
        int     _picn;
        int     _mode;
        int     _ver;
        float*  _profile;

        PicRequest() { _picn = -1; }
        bool valid() const { return _picn >= 0; }
    };
    
    PicRequest          _lastPic;

    IFMagnetic() { _init(); }

    ~IFMagnetic()
    {
        stop(); 
        _purge();
    }

    // compliance
    bool start(const char* configDir,
               const char* dataDir,
               const char* story,
               charEmitFn* transcriptEmit, void* tctx,
               charEmitFn* consoleEmit, void* cctx,
               segmentInfoFn* sfn, void* sctx) override;

    void initialCommands();
    void restartCommands();
    void gameStarting();
    void gameRestarting();

    void emitScene() override 
    {
        if (_sync())
        {
            flush();
            _syncLock.unlock();
        }
    }

    bool setOptions(const VarSet&) override;

    static void _emits(charEmitFn* emitter, void* ctx, const char* s)
    {
        while (*s) (*emitter)(ctx, *s++);
    }

    void promptConsole() override 
    {
        static const char normal_prompt[] = "> ";
        _emits(_consoleEmit, _cctx, normal_prompt);
    }

    bool  evalUseXwithY(const string& x, const string& y,
                        CommandResultI* res = 0) override;

    bool evalUseX(const string&, bool frommenu);
    bool puzzleForX(const string* adj, const string& x, int act);

    CommandResultI* makeResult() override { return new CommandResult(); }
    
    void tidyTranscript();
    void parseTranscript();

    void clearImage() override
    {
        delete [] _imageJSONBuf;


        GrowString gs;
        gs.append("{}");
        gs.add(0);
        _imageJSONBuf = gs.donate();

        LOG3("MS clear image", _imageJSONBuf);
        
        flushImage();
    }

    void flushImage()
    {
        if (_imageJSONBuf)
        {
            // there's an image waiting
            (_segmentInfoEmit)(_sictx, BRA_SEGMENT_IMAGE);

            _emits(_transcriptEmit, _tctx, _imageJSONBuf);
            (*_transcriptEmit)(_tctx, 0);
            
            (_segmentInfoEmit)(_sictx, 0);

            // clear
            delete [] _imageJSONBuf;
            _imageJSONBuf = 0;
        }
    }
    
    void flush()
    {
        if (_outpos)
        {
            // terminate
            _outbuf[_outpos] = 0;
            _outpos = 0;

            // clean up
            tidyTranscript();

            // transform
            parseTranscript();
            
            // emit
            _emits(_transcriptEmit, _tctx, _outbuf);
            
            // end of section
            (*_transcriptEmit)(_tctx, 0);
        }

        if (_statusReady)
        {
            (_segmentInfoEmit)(_sictx, BRA_SEGMENT_TITLE);            
            
            _emits(_transcriptEmit, _tctx, _statusBuf);
            (*_transcriptEmit)(_tctx, 0);
            
            (_segmentInfoEmit)(_sictx, 0);            

            // collect the next line
            _statusReady = false; 
            _statusPos = 0;
        }
        
        flushImage();
    }

    string transformCommand(const char* cmd) const;
    bool evalCommandSpecial(const char* cmd, CommandResultI* cres);
    bool _evalCommand(const char* cmd,
                      CommandResultI* res = 0, 
                      bool echo = false);
    
    bool evalCommand(const char* cmd,
                     CommandResultI* cres = 0,
                     bool echo = false) override
    {
        string c = transformCommand(cmd);
        return evalCommandSpecial(c.c_str(), cres)
            || _evalCommand(c.c_str(), cres, echo);
    }

    void setLogLevel(int level)
    {
        if (level >= 0 && level < 100)
        {
            Logged::_logLevel = level;
            LOG2("MS, log level ", level);
        }
    }

    bool updateMapInfo(MapInfo& mi) override;
    bool updateRosterInfo(RosterInfo& ri) override;
    bool getProductInfo(ProductInfo&) override;
    string currentVersion() const override;

    void stop()
    {
        LOG3("MS ", "stopping");
        _shutdown = true;
        _msTask.join();
    }

    void _init()
    {
        Logged initLog;
        
        _running = false;
        _shutdown = false;
        _starting = false;
        _waiting = false;
        _inputReady = false;

        _createImageDir = true;
        _addCommandNewline = true;
        
        _ms_gfx = 0;
        _outpos = 0;

        _inpos = 0;
        _inbuf[0] = 0;
        _imageJSONBuf = 0;

        _statusPos = 0;
        _statusBuf[0] = 0;
        _statusReady = false; // says when to flush

        _gameSaveMemory = 0;
        _gameSaveMemorySize = 0;
        
        _puzzlesEnabled = true; // modern default
        _useXBR4 = true;
        _ignoreOutput = false;
    }

    void _purge()
    {
        delete [] _imageJSONBuf;
    }

    void runMsTask();

    void waitForInput()
    {
        std::unique_lock<mutex> lock(_syncLock);
        
        _waiting = true;

        //LOG4("MS, ", "waiting for input");

        while (!_inputReady && !_shutdown)
            _waiter.wait_for(lock, std::chrono::milliseconds(500));

        _inputReady = false;
        _waiting = false;
    }

    // task methods
    void emitchar(char c)
    {
        // NB: ms thread
        
        if (c == '\b')
        {
            if (_outpos) _outbuf[--_outpos] = 0;
        }
        else
        {
            if (_outpos < sizeof(_outbuf)-1)
                _outbuf[_outpos++] = c;
        }
    }

    void tidyStatusText();

    void _putStatusChar(char c)
    {
        if (!_statusReady)
        {
            if (c == '\n' || c == '\r')
            {
                // end of line. status is ready to send in next flush
                if (_statusPos > 0)
                {
                    _statusBuf[_statusPos] = 0; // terminate
                    _statusReady = true; // wait for send

                    tidyStatusText();

                    //LOG3("MS status, '", _statusBuf << "'");
                }
            }
            else
            {
                if (_statusPos < sizeof(_statusBuf)-1)
                    _statusBuf[_statusPos++] = c;
            }
        }

    }

    char _getchar()
    {
        // NB: ms thread

        if (_puzzles._triggerUndo)
        {
            //LOG3("getchar, ", "trigger undo");
            _inpos = 0;
            _inbuf[0] = 0;
            _puzzles._triggerUndo = false;
            return 0; // signal emu to undo move
        }
        
        char c;
        do
        {
            c = _inbuf[_inpos];
            if (c)
            {
                // some buffer to return
                ++_inpos;
            }
            else
            {
                waitForInput();
            }
        } while (!c && !_shutdown);
        
        return c;
    }

    const char*  messageHook(int msg)
    {
        // called when a `msg` is printed
        // pass to game specific puzzle handlers
        // NB: game thread
        return _puzzles.messageHook(msg);
    }

    void _showpic(const PicRequest*);

    void updateGameSaveArea(uchar* ptr, size_t size);
    bool loadGame(const char* name, uchar* ptr, size_t size);
    bool saveGame(const char* name, uchar* ptr, size_t size);
    bool saveGame(const char* name, SaveGameHeader& h);
    bool loadGame(const char* name, SaveGameHeader& h);

    bool updateAutoSave();
    void moveUpdate(int movecount);
    bool autoLoad(int delta);
    bool loadMemGame(const char* name, bool forceLook, bool clearpic);

    bool saveGame(const char* name) override
    {
        bool res = _gameSaveMemory != 0;
        if (res)
        {
            SaveGameHeader shead;
            
            // mark as a memory image
            shead._type = SAVE_TYPE_MEMIMAGE;

            // raw save game file
            res = saveGame(name, shead);        
        }
        return res;
    }
    
    bool loadGame(const char* name) override 
    {
        // called from UI
        bool res = _gameSaveMemory != 0;
        if (res) res = loadMemGame(name, false, true);
        return res;
    }

    bool clearImageCache();

private:

    void _buildRosterJSON(GrowString& buf);
    void _buildMapJSON(GrowString& buf);
    void _buildPictureJSON(GrowString& buf,
                           const string& imagepath, 
                           const PicRequest*);

    bool _substWordID(string& word);
    bool _substWordsID(std::vector<string>& words);
    bool _evalCommandAux(const char* cmd, CommandResultI* res);
    bool _sync();

};
