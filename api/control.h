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

#include <vector>
#include "apitypes.h"
#include "prefs.h"
#include "transcript.h"
#include "console.h"
#include "mapbox.h"
#include "itemsmodel.h"
#include "os.h"
#include "json.h"
#include "ap.h"
#include "utils.h"

// forward
class IFEngineInterface;
struct MapInfo;
struct var;

struct DLLX ControlOptions
{
    ControlOptions()
    {
        _hiDpi = false; // default off
        
        // sometimes we can figure out the true dpi
        _useAccurateDPI = false;
    }

    bool        _hiDpi;
    bool        _useAccurateDPI;
};

struct DLLX Control: public APIType
{
    Control() { _init(); }
    
    virtual ~Control();

    typedef Strings Words;

    Prefs::Ref                  _prefs;
    Transcript::Ref             _transcript;
    Console::Ref                _console;
    Map::Ref                    _map;

    // if defined by options
    string                      _storyfile;
    string                      _engineName;
    string                      _musicFile;
    AP<JSON>                    _productInfoJSON;

    ItemsModel::Ref             _itemsModel;

    // JSON models
    string                      _currentImageJSON;
    string                      _currentMetaJSON;
    string                      _soundJSON;
    string                      _ifiChoiceJSON;

    // datapath is per user data
    string                      _dataPath;

    // assets are part of release
    string                      _assetsPath;

    ControlOptions              _options;

    // if suggested by command-line options
    bool                        _simMode = false;
    bool                        _simLargeMobile = false;
    bool                        _uiNoFrame = false;
    int                         _uiOptionWidth = 0;
    int                         _uiOptionHeight = 0;
    int                         _uiOptionFontSize = 0;
    double                      _uiOptionSimFontSizeMultiplier = 1;
    
    virtual void handleOptions(int argc, char** argv);
    void startUp(const char* dataPath,
                 const char* configDir,
                 int userSpaceID);

    void beginGame();
    void coverPageClosed();

    bool loadEngine(IFEngineInterface*);
    bool loadIFI();
    string prefsFilePath() const;
    
    string currentVersion() const;
    virtual void setLogLevel(int level);
    virtual int getLogLevel() const;


    bool evalCommand(const string& cmd);
    bool evalClickCommand(const string& cmd);
    bool refreshCommand();
    bool evalCommandDirect(const string& cmd, bool echo);
    bool evalUseXwithY(const string& x, const string& y);
    bool menuDrop(const string&);
    bool menuClick(const string&);
    
    bool getProductInfo();
    int  getScreenDPI() const;
    static string selectWords(const string& s, int start, int end);

    // overridden by QControl
    virtual void imageChanged(const string& js) {}
    virtual void metaChanged(const string& metajs) {}
    virtual void soundChanged(const string& js) {}
    virtual void ifiChoiceChanged(const string& js) {}

    bool updateMapInfo(MapInfo&);
    void evalItemList(ItemsModel::Entries&);
    void evalPeopleList(ItemsModel::Entries&);
    string undoredo(bool undo);
    bool sendOptionToEngine(const string& opt, const var& val);

    bool saveGame(const string& name);
    bool loadGame(const string& name);
    bool restartGame();
    bool deleteSaveGame(const string& name);

    void postEval();
    void echoConsoleToTranscript(bool);
    void updatePrefDependencies();
    void suggestCompletion(const char* w, Words&);

    string itemIcon(const string& id);
    string resolveAsset(const string& id) const;
    string nameFromId(const string& id) const;
    bool compassDirection(uint dir);

    string titleText() const;
    virtual bool titleText(const string&);

    enum Dir 
    {
        dir_n = 0,
        dir_ne,
        dir_e,
        dir_se,
        dir_s,
        dir_sw,
        dir_w,
        dir_nw,
        dir_u,
        dir_d,
        dir_in,
        dir_out,
        dir_count,
    };

    static const char* dirName(int dir)
    {
        static const char* dirNames[] =
            {
                "north",
                "northeast",
                "east",
                "southeast",
                "south",
                "southwest",
                "west",
                "northwest",
                "up",
                "down",
                "in",
                "out"
            };

        if (dir < 0 || dir >= ASIZE(dirNames)) dir = dir_in;
        return dirNames[dir];
    }
    

private:

    struct Imp;
    Imp*                _imp;

    void _init();
    
};

