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

#include <QImage> 
#include "ifmagnetic.h"
#include "strutils.h"
#include "qdefs.h"
#include "map.h"
#include "markup.h"
#include "varset.h"
#include "ifproduct.h"
#include "ifpic.h"
#include "filters.h"


#define VERSION_STRING  "4"

static IFMagnetic* theMS;

static int stop_restart_hook(int r, void* ctx, int request)
{
    // this hook is called twice, once before and once after
    // beforehand `request`==1 otherwise 0.
    
    if (!request)
    {
        // make adjustments after restart
        ((IFMagnetic*)ctx)->gameRestarting();
    }

    return 1; // always restart
}

bool IFMagnetic::start(const char* configDir,
                       const char* dataDir,
                       const char* story,
                       charEmitFn* transcriptEmit, void* tctx,
                       charEmitFn* consoleEmit, void* cctx,
                       segmentInfoFn* sfn, void* sctx)
{

    // look for prog first, otherwise mag
    _story = changeSuffix(story, ".prog");
    if (!FD::exists(_story.c_str())) _story = changeSuffix(story, ".mag");
    
    _configDir = configDir;
    _dataDir = dataDir;

    LOG3("Story, ", _story);
    LOG4("MS, configDir ", _configDir);
    LOG4("MS, dataDir ", _dataDir);
    
    if (_createImageDir)
    {
        // images stored in a subdirectory
         _imageDir = _dataDir + "/images";

         if (!FD::mkdirIf(_imageDir.c_str()))
             LOG("ERROR: failed to create image dir ", _imageDir);
        
        LOG3("MS, imageDir ", _imageDir);
    }

    // emitter functions not to be called on mstask thread
    _transcriptEmit = transcriptEmit; 
    _tctx = tctx;
    _consoleEmit = consoleEmit; 
    _cctx = cctx;
    _segmentInfoEmit = sfn;
    _sictx = sctx;

    // assign to global so that emulator handlers can get back to us
    theMS = this;

    // save game directory
    _undos.setup(_dataDir);

    if (_undos.size())
    {
        LOG4("Undos ", _undos);
        
        // if we have an autosave, throw all initial output away.
        // see comment in `initialCommands`
        (_segmentInfoEmit)(_sictx, BRA_SEGMENT_NULL);

        _ignoreOutput = true;
    }

    // cause game to restart on quit
    set_stop_or_restart_hook(stop_restart_hook, this);

    // interval between starting thread and it running
    _starting = true;
    
    _msTask = std::thread(&IFMagnetic::runMsTask, this);

    // send initial setup commands to game
    initialCommands();

    // load puzzles after initial commands. this is to ensure game
    // has started. assume puzzles solving isn't needed by initial commands!
    _puzzles.start(this);
    
    return true;
}

void IFMagnetic::gameStarting()
{
    // NB: on game thread
    
    //LOG3("MS ", "starting");
    _puzzles.reset();
}

void IFMagnetic::gameRestarting()
{
    gameStarting();

    // need game to be operational to apply fixes
    _puzzles.applyGameFixes();
}

void IFMagnetic::runMsTask()
{
    const char* story = _story.c_str();
    _ms_gfx = ms_init((type8s*)story,
                      (type8s*)changeSuffix(_story, ".gfx").c_str(), 0, 0);

    if (!_ms_gfx)
    {
        LOG("MS ", "failed to load story '" << _story << "'");
        return;
    }

    // hook messages back to us for special cases
    _messages._hook = std::bind(&IFMagnetic::messageHook,
                                this, 
                                std::placeholders::_1);

    _messages.start(story);

    --_ms_gfx;
    _running = true;
    _starting = false; // we've started now we're runnning

    LOG3("MS ", "running");    

    gameStarting();

    while (_running)
    {
        _running = !_shutdown && ms_rungame() && ms_is_running();
    }

    LOG3("MS ", "shutdown");    
    
    //ms_freemem();
}

bool IFMagnetic::_sync()
{
    // wait until emulator quiet, waiting on input
    //TimeReport syncm("sync");
    int cc1 = 0, cc2 = 0;
    for (;;)
    {
        _syncLock.lock();
            
        if (_waiting && !_inputReady)
        {
            break;
        }
        else
        {
            _syncLock.unlock();

            // has started and we're no longer running
            // game has terminated
            if (!_running && !_starting) return false;
            
            if (++cc1 >= 25)
            {
                ++cc2;
                LOG3("_sync, too long ", cc2*500);
                cc1 = 0;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    }
    return true;
}

static void builderEmitter(char c, void* ctx)
{
    GrowString* buf = (GrowString*)ctx;
    buf->add(c);
}

void IFMagnetic::_buildPictureJSON(GrowString& buf, 
                                   const string& filepath,
                                   const PicRequest* pr)
{
    // use builder helper, give it our emitter
    PictureInfoBuilder build(builderEmitter, &buf);

    build.beginPic();

    build.picName(filepath);

    const float* profile = pr->_profile;
    if (profile)
    {
        build.picContast(profile[0]);
        build.picBrightness(profile[1]);
        build.picLightness(profile[2]);
        build.picSaturation(profile[3]);
        build.picGamma(profile[4]);
    }

    build.endPic();
    
    buf.add(0);  // terminate string
}

bool IFMagnetic::clearImageCache()
{
    // delete all images in `imageDir`
    std::vector<string> files;
    bool r = FD::getDirectory(_imageDir.c_str(), files);
    if (r)
    {
        for (size_t i = 0; i < files.size(); ++i)
        {
            // the images are all PNG
            if (equalsIgnoreCase(suffixOf(files[i]), ".png"))
            {
                const string& fi = files[i];

                string imagepath = _imageDir + '/' + fi;
                if (FD::remove(imagepath.c_str()))
                {
                    LOG2("MS, removed cache image ", imagepath);
                }
                else
                {
                    LOG1("WARNING: unable to remove cache image ", imagepath);
                }
            }
        }
    }
    else
    {
        LOG1("MS, unable to list directory ", _imageDir);
    }
    return r;
}

void IFMagnetic::_showpic(const PicRequest* pr)
{
    if (pr->_ver <= 1)
    {
        LOG3("MS showpic #", pr->_picn << " mode " << pr->_mode);
    }

    char imagename[128];

    // prefix
    strcpy(imagename, "ms_");
    
    if (pr->_ver > 1)
    {
        const char* p = (const char*)getcode() + pr->_picn;
        LOG3("MS show pic name: ", p << " mode " << pr->_mode);

        char* q = imagename + strlen(imagename);
        while (*p)
        {
            if (*p == '.') break;
            *q++ = u_tolower(*p);
            ++p;
        }
        *q = 0;
    }
    else sprintf(imagename + strlen(imagename), "image%d", pr->_picn);

    if (_ignoreOutput)
    {
        LOG3("ignoring picture ", imagename);
        return;
    }
    
    if (pr->_mode == 0)
    {
        LOG3("MS hidepic ", imagename);
        return;
    }

    if (_useXBR4)
    {
        // use a different name for processed images
        strcat(imagename, "_xbr4");
    }
    
    strcat(imagename, ".png");
    //LOG3("pic save name: ", imagename);

    _lastPic = *pr;

    // exists or not
    string imagepath = _imageDir + '/' + imagename;

    bool ok = true;

    if (!FD::exists(imagepath.c_str()))
    {
        // decode picture and save it

        type16 w, h;
        type16 pal[16];
        type8 isAnim;
    
        uchar* p = ms_extract(pr->_picn, &w, &h, pal, &isAnim);
        if (p)
        {
            //LOG3("MS ", "pic size " << w << "x" << h);

            if (w > 0 && w < 1024 && h > 0 && h < 1024)
            {
                uchar* raw = new uchar[w*h*4];
                uchar* q = raw;
                
                int stride = w;
                for (int y = 0; y < h; ++y)
                {
                    for (int x = 0; x < w; ++x)
                    {
                        uint v = p[stride*y + x];
                        
                        v = pal[v & 0xf];

                        *q++ = (v&0x000F)<<5; // blue
                        *q++  = (v&0x00F0)<<1; // green
                        *q++ = (v&0x0F00)>>3; // red
                        *q++ = 0xff; // alpha
                    }
                }

                int scaleFactor = 1;
                
                if (_useXBR4)
                {
                    scaleFactor = 4;
                    uchar* outBuffer = new uchar[w * scaleFactor * h * scaleFactor * 4];
                    xbr_params xbrParams;
                    xbrParams.input = raw;
                    xbrParams.output = outBuffer;
                    xbrParams.inWidth = w;
                    xbrParams.inHeight = h;
                    xbrParams.inPitch = w * 4;
                    xbrParams.outPitch = w * scaleFactor * 4;

                    switch (scaleFactor)
                    {
                    case 2: xbr_filter_xbr2x(&xbrParams); break;
                    case 3: xbr_filter_xbr3x(&xbrParams); break;
                    case 4: xbr_filter_xbr4x(&xbrParams); break;
                    }

                    delete [] raw;

                    raw = outBuffer;
                }
                
                QImage* img = new QImage(w*scaleFactor, h*scaleFactor,
                                         QImage::Format_RGB32);
                QRgb* pix = (QRgb*)img->bits();
                memcpy(pix, raw, w*scaleFactor * h*scaleFactor * 4);

                delete [] raw;

                LOG3("MS showpic, saving ", imagepath);
                img->save(imagepath.c_str(), "PNG");
                delete img;
            }
            else ok = false;
        }
        else
        {
            LOG1("MS could not find picture ", imagename);
            ok = false;
        }
    }

    if (ok)
    {
        // build picture json
        GrowString buf;
        _buildPictureJSON(buf, imagepath, pr);

        // takes out the string and gives it to us
        _imageJSONBuf = buf.donate();
    }
}

bool IFMagnetic::_evalCommand(const char* cmd, CommandResultI* cres, bool echo)
{
    bool res = false;

    if (cmd && *cmd)
    {
        if (!_sync()) return false;

        bool capture = cres && cres->_op == CommandResultI::op_capture;
        if (capture)
        {
            // we are going to capture into the same output buffer
            // collect and return. first flush any residual content
            flush();
        }

        if (echo)
        {
            _emits(_consoleEmit, _cctx, cmd);

            // flush
            (*_consoleEmit)(_cctx, '\n');
            (*_consoleEmit)(_cctx, 0);
        }
        
        res = _evalCommandAux(cmd, cres);

        if (capture)
        {
            CommandResult* cr = (CommandResult*)cres;

            // above _eval does not synchronously eval. so force it.
            if (!_sync()) return false;

            // collect any output
            if (_outpos)
            {
                // terminate
                _outbuf[_outpos] = 0;
                _outpos = 0;     

                // cleanup
                tidyTranscript();
                
                cr->_result = _outbuf;
            }

            _syncLock.unlock(); // release
        }
        
    }
    return res;
}

bool IFMagnetic::_evalCommandAux(const char* cmd, CommandResultI* res)
{
    // NB: ONLY call after sync (assumes lock)
    
    LOG3("ms, eval command aux \"", cmd << "\"");

    const int _inbufsz = sizeof(_inbuf);

    if (_inbufsz > 0)
    {
        strncpy(_inbuf, cmd, _inbufsz-1);
        _inbuf[_inbufsz-1] = 0; // ensure termination
        _inpos = 0;

        if (_addCommandNewline && _inbufsz > 2)
        {
            // if engine expects commands to end with a newline
            size_t sz = strlen(_inbuf);
            if (sz && _inbuf[sz-1] != '\n' && sz < _inbufsz-2)
            {
                _inbuf[sz++] = '\n';
                _inbuf[sz] = 0;
            }
        }
    }

    _inputReady = true;

    // wake it up.
    _waiter.notify_one();
                
    _syncLock.unlock();

    return true;
}

std::string IFMagnetic::transformCommand(const char* cmd) const
{
    // apply textual changes to input commands
    
    assert(cmd);

    bool changed = false;
    
    // first split into separate commands. 
    // separated by "then" or period.
    std::vector<string> cmds;
    split(cmds, replaceAll(toLower(cmd), "then", "."), '.');
    for (size_t i = 0; i < cmds.size(); ++i)
    {
        // for each command, look for transforms and mark changed
        string si = trim(cmds[i]);
        //LOG3("cmd ", i << "; " << si);

        std::vector<string> words;
        split(words, si);

        bool cw = false;

        if (words.size())
        {
            const string& verb = words[0];
            if (verb == "x")
            {
                words[0] = "examine"; 
                cw = true;
            }
            else if (verb == "quit")
            {
                // prevent quit being entered
                words[0] = "restart";
                cw = true;
            }
            
            if (cw) si = unsplit(words);
        }

        if (cw) changed = true;
        cmds[i] = si;
    }

    if (changed)
    {
        // rebuild
        return unsplit(cmds, ". ");
    }
    else return cmd;
}

bool IFMagnetic::_substWordID(string& word)
{
    bool r = false;
    if (startsWith(word, "item"))
    {
        int id = atoi(word.c_str() + 4);
        if (id > 0)
        {
            // this is an ID ref
            IItem ii(find_item(id));
            if (ii)
            {
                word = ii.adjWord();
                LOG4("subst ID ", id << " -> " << word);
                r = true;
            }
        }
    }
    return r;
}

bool IFMagnetic::_substWordsID(std::vector<string>& words)
{
    size_t n = words.size();
    bool r = false;
    for (size_t i = 0; i < n; ++i)
        if (_substWordID(words[i])) r = true;
    
    return r;
}

bool IFMagnetic::evalCommandSpecial(const char* cmd, CommandResultI* cres)
{
    // handle special cases
    assert(cmd);

    CommandResult* cr = (CommandResult*)cres;

    // handle undo/redo first. other commands reset the undo state
    if (!u_stricmp(cmd, "undo"))
    {
        bool v = autoLoad(-1);
        LOG3("Undo: ", _undos);
        
        if (cr)
            cr->_result = v ? "undo " + std::to_string(_undos.position()) : "no more undo";
        
        return true;
    }
    else if (!u_stricmp(cmd, "redo"))
    {
        bool v = autoLoad(1);
        LOG3("Redo: ", _undos);

        if (cr)
        {
            if (v)
            {
                int p = _undos.position();
                cr->_result = "redo ";
                if (p) cr->_result += std::to_string(p);
                else cr->_result += " last";
            }
            else cr->_result = "no more redo";
        }
        
        return true;
    }

    bool res = false;

    // as soon as a non-undo/redo is issued, we accept the undo state
    _undos.acceptCurrent();

    std::vector<string> words;
    split(words, cmd);
    size_t n = words.size();

    if (!n) return true; // sanity

    _substWordsID(words);

    // handle meta commands
    if (equalsIgnoreCase(words[0], "do"))
    {
        // do <something> <n>
        // last word is a number when we want to indicate a puzzle state

        // take off any number of the end
        int act = atoi(words[n-1].c_str());
        if (act > 0) --n;

        if (n > 0)
        {
            const string* adj = 0;
            if (n > 2) adj = &words[n-2];
            
            // first look for a puzzle bound to the root word
            res = _puzzlesEnabled && puzzleForX(adj, words[n-1], act);

            if (!res)
            {
                // otherwise useX (NB: drop "do")
                res = evalUseX(unsplit(words, ' ', 1, n), false);
            }
        }
    }
    else if (equalsIgnoreCase(words[0], "domenu"))
    {
        res = evalUseX(unsplit(words, ' ', 1, n), true);
    }
    else if (equalsIgnoreCase(words[0], "goto")) 
    {
        // goto command can be
        // goto room#  where we figure out the route  OR
        // goto room1 room2 room3   where the route is given

        // evaluate direction moves until fail. 
        for (size_t i = 1; i < words.size(); ++i)
        {
            if (!_puzzles.goroom(atoi(words[i].c_str())))
                break;
        }
        
        res = true;
        
    }
    return res;
}

bool IFMagnetic::puzzleForX(const string* adj, const string& x, int act)
{
    // look for a puzzle for x
    // if adj and no puzzle for x, look for a puzzle for "adj x".
    
    bool res = false;
    
    Puzzle* p = _puzzles.find(x.c_str());

    if (!p && adj)
    {
        // try adjective as well
        string ax = *adj + ' ' + x;
        p = _puzzles.find(ax.c_str());        
    }

    if (p)
    {
        LOG3("MS puzzle for '", x << "' act " << act);

        // if we satisfy the precondition, accept puzzle regardless
        res = p->precondition();
        if (res)
        {
            p->_res = true;
            if (act)
            {
                p->action(act);

                // if action didn't happen, then puzzle failed
                res = p->_res;
            }
            else
            {
                p->run();

                // if nothing handled, recycle state
                if (p->_res) 
                {
                    p->_state = 0; 

                    // if run didn't clear res state, then we didn't do
                    // anything, therefore fail the puzzle.
                    res = false;
                }
            }
        }
    }
    return res;
}

bool IFMagnetic::evalUseX(const string& x, bool frommenu)
{
    // find appropriate command for object x

    LOG3("MS use '", x << "'");

    IItem xi(x);
    string command;
    
    bool res = xi;
    if (res)
    {
        //xi.expose();

        if (frommenu)
        {
            PuzzleBase p(&_puzzles);

            // force drop, even if not carried
            // we want the error
            p.actDrop(xi, true);

            // handled either way
            res = true;
        }
        else
        {
            PChooser::Seq seq;

            seq.add("look");
            
            if (xi.gettable() && !xi.carried())
            {
                seq.add("get");
            }
            if (xi.isClosed() || xi.isLocked())
            {
                seq.add("open");
            }
            if (xi.isWearable() && !xi.isWorn())
            {
                seq.add("wear");
            }
            if (xi.isContainer())
            {
                seq.add("look in");
            }
            if (xi.isDoor() && xi.isOpen())
            {
                seq.add("go");
            }
            if (xi.isSurface())
            {
                seq.add("look on");
            }
            if (xi.isBigThing() && xi.isMoveable())
            {
                seq.add("look under");
            }

            command = _puzzles._pchooser.choose(xi, seq);

            command += ' ';
            command += xi.toString();
            LOG3("useX: ", command);

            res = _evalCommand(command.c_str(), 0, true); // always echo
        }
    }
    return res;
}

bool IFMagnetic::evalUseXwithY(const string& x, const string& y,
                               CommandResultI* cres)
{

    // decode items specified by ID
    string x1 = x;
    _substWordID(x1);
    
    string y1 = y;
    _substWordID(y1);

    LOG3("MS useXY \"", x1 << "\" with \"" << y1 << "\"");

    IItem xi(x1);
    IItem yi(y1);
    string cmd; 

    bool res = xi && yi;

    if (res)
    {
        LOG3("MS useXY \"", xi << "\" with \"" << yi << "\"");

        cmd = _puzzles.evalUseXwithYSpecial(xi, yi);

        if (cmd.empty())
        {
            if (yi.isNPC())
            {
                if (xi.isNPC())
                {
                    cmd = "ask " + yi.toString() + " about " + xi.toString(); 
                }
                else
                {
                    cmd = "give " + xi.toString() + " to " + yi.toString();
                }
            }
            else if (xi.isKey())
            {
                if (yi.isLocked())
                {
                    cmd = "unlock " + yi.toString() + " with " + xi.toString();
                }
            }
            else if (xi.isNPC() && !xi.isDead())
            {
                // yi is not NPC
                if (xi.reachable())
                {
                    // drag an NPC reference onto a thing asks them about it
                    cmd = "ask " + xi.toString() + " about " + yi.toString();   
                }
            }
            else if (xi.isMoney() && yi.gettable())
            {
                cmd = "buy " + yi.toString() + " with " + xi.toString();   
            }
        }

        if (cmd.empty())
        {
            // default operation
            if (yi.isContainer())
            {
                cmd = "put " + xi.toString() + " in " + yi.toString();
            }
            else if (xi.isContainer() && yi.gettable())
            {
                cmd = "put " + yi.toString() + " in " + xi.toString();
            }
            else
            {
                if (xi.isTieable() && yi.canTieTo())
                {
                    cmd = "tie " + xi.toString() + " to " + yi.toString();
                }
                else
                {
                    // default
                    cmd = "put " + xi.toString() + " on " + yi.toString();
                }
            }
        }

        res = _evalCommand(cmd.c_str(), cres, true); // always echo
    }
    return res;
}


void IFMagnetic::_buildMapJSON(GrowString& buf)
{
    // build a JSON string of the map data & layout

    typedef GameMap::MapPos MapPos;
    typedef std::vector<MapPos> MapPosSet;

    GameMap& gm = GameMap::theMap;
    
    // use builder helper, give it our emitter
    MapInfoBuilder build(builderEmitter, &buf);

    build.begin();

    // build a collection rooms we know about
    MapPosSet knownMapPos;
    Point2 minpos;
    
    for (size_t i = 0; i < gm.size(); ++i)
    {
        // each room PPS gives a grid position
        const MapPos& mpi = gm.at(i);

        IItem ri = IItem::getRoom(mpi.room);

        bool allMapCheat = false;

#ifdef LOGGING
            // logging level >= 2, add room# to name 
            if (Logged::_logLevel >= 2) allMapCheat = true;
#endif

        // collect only rooms we know about
        if (ri && (ri.isExplored() || allMapCheat))
        {
            // collect the min grid locations
            if (knownMapPos.size()) minpos = minpos.min(mpi.pos);
            else minpos = mpi.pos;
            knownMapPos.push_back(mpi);
        }
    }

    build.beginPlaces();
    for (size_t i = 0; i < knownMapPos.size(); ++i)
    {
        MapPos& mpi = knownMapPos[i];
        
        // normalise grid positions, adjust to (0,0)
        mpi.pos -= minpos;

        IItem ri = IItem::getRoom(mpi.room);

        build.beginPlace();
        build.placeID(mpi.room);
        build.placeName(ri.toString());
        build.placeX(mpi.pos.x);
        build.placeY(mpi.pos.y);

        // exits
        int exits[ITEM_MAX_EXITS];
        int ne = ri.getExitRooms(exits);

        if (ne > 0)
        {
            build.beginExits();
            for (int i = 0; i < ne; ++i) build.exit(exits[i]);
            build.endExits();
        }
        
        build.endPlace();
    }

    build.endPlaces();
    build.end();
    buf.add(0);  // terminate string
}

bool IFMagnetic::updateMapInfo(MapInfo& mi)
{
    LOG3("MS ", "updateMapInfo");
     
    IItem croom = IItem::currentRoom();

    mi._currentLocation = croom.roomNumber();
    mi._currentExits = 0;
    
    uint dmask = 1;
    for (int d = IItem::dir_n; d < IItem::dir_count; ++d, dmask <<= 1)
        if (croom.getExit(d)) mi._currentExits |= dmask;
    
    if (mi._full)
    {
        // build full map data
        GrowString buf;
        _buildMapJSON(buf);
        mi._changed = mi._json != buf.start();
        if (mi._changed) mi._json = buf.start();
    }
    return true;
}

void IFMagnetic::_buildRosterJSON(GrowString& buf)
{
    // use builder helper, give it our emitter
    RosterInfoBuilder build(builderEmitter, &buf);

    build.begin();

    IItem::IItems items;
    IItem::getInventory(items);
    
    build.beginItems();
    for (size_t i = 0; i < items.size(); ++i)
    {
        const IItem& ii = items[i];
        build.beginItem();
        build.itemName(_puzzles.itemToStringSpecial(ii));
        build.itemID(ii.id());
        if (ii.isWorn()) build.itemWorn();
        build.endItem();
    }
    build.endItems();

    build.beginPeople();
    for (size_t i = 1; i <= itemCount; ++i)
    {
        IItem ii(find_item(i));
        if (ii.isNPC() && ii.isExplored() && !ii.isAnimal())
        {
            build.beginPerson();
            build.personName(_puzzles.itemToStringSpecial(ii));
            build.personID(ii.id());
            build.endPerson();
        }
    }
    build.endPeople();
    
    build.end();
    buf.add(0);  // terminate string
}

bool IFMagnetic::updateRosterInfo(RosterInfo& ri)
{
    //LOG3("MS ", "updateRosterInfo");
     
    GrowString buf;
    _buildRosterJSON(buf);
    
    ri._changed = ri._json != buf.start();
    if (ri._changed) ri._json = buf.start();
    return true;
}

bool IFMagnetic::getProductInfo(ProductInfo& pi)
{
    LOG3("MS, ", "getProductInfo");

    CommandResult cr;
    cr._op = CommandResultI::op_capture;
    if (_evalCommand("credits", &cr))
        _originalCredits = trim(cr._result);

    // product info is sources from PuzzleManager
    // so to keep all the game specific dependencies there.
    GrowString gs;
    _puzzles.buildProductInfoJSON(gs, _originalCredits);

    pi._changed = true;
    pi._json = gs.start();
    return true;
}

std::string IFMagnetic::currentVersion() const
{
    return VERSION_STRING;
}

void IFMagnetic::moveUpdate()
{
    updateAutoSave();
    _puzzles.moveUpdate();
}

bool IFMagnetic::updateAutoSave()
{
    // NB: on engine thread
    bool res = _gameSaveMemory != 0 && _undos.canAutoSave();
    if (res)
    {
        SaveGameHeader shead;

        // mark autosave as a memory image
        shead._type = SAVE_TYPE_MEMIMAGE;

        // next save name
        string sname = _undos.autosaveName(_undos._last + 1);
        
        // raw save game file
        res = saveGame(sname.c_str(), shead);        

        if (res) _undos.advanceLast(); 

    }
    return res;
}

bool IFMagnetic::loadMemGame(const char* name, bool forceLook, bool clearpic)
{
    SaveGameHeader shead;

    // mark as requiring a mem image
    shead._type = SAVE_TYPE_MEMIMAGE;

    bool res = loadGame(name, shead);

    if (res && clearpic)
    {
        // clear any old pic before look
        clearImage();
    }

    if (res || forceLook)
    {
        UndoState::SuspendSave ss(_undos);
        
        // force a "look"
        _evalCommand("look");

        emitScene(); // sync
        
        IItem cr = IItem::currentRoom();
        if (cr) 
        {            
            // it appears that sometimes the current room is not explored
            // after a load. this otherwise breaks the map
            cr.setExplored();
        }
    }
    
    return res;
}

bool IFMagnetic::autoLoad(int delta)
{
    bool res = false;
    
    if (!delta || _undos.deltaCurrent(delta))
    {
        // NB: if load fails (eg corrupt file) continue to force a look
        // otherwise get blank screen. will be fixed once game writes new
        // autosave
        res = loadMemGame(_undos.autosaveName(0).c_str(), true, false);
    }
    
    return res;
}

void IFMagnetic::restartCommands()
{
    // issues commands to the game after restarted
    
    CommandResult* cr = (CommandResult*)makeResult();

    cr->_op = CommandResultI::op_capture;    

    static const char* setupCommands[] = 
    {
        "verbose",
    };
    
    for (size_t i = 0; i < DIM(setupCommands); ++i)
    {
        _evalCommand(setupCommands[i], cr);
        LOG4("MS, command: ", setupCommands[i] << ",  '" << cr->toString() << "'");
    }

    delete cr;
}

void IFMagnetic::initialCommands()
{
    // issue commands to the game on first start
    
    CommandResult* cr = (CommandResult*)makeResult();

    cr->_op = CommandResultI::op_capture;    

    /* if we start with an autosave, a "load" command doesn't refresh
     * the screen, furthermore, we also get the initial game spiel.
     * 
     * as a workaround, we gag the output (by sending it to NULL) in
     * setup, then here we perform the load, then we un-gag the output
     * and perform a "look". this generates current output, title and
     * picture.
     *
     * if there was no autosave, we still perform the "load", but it
     * fails silently. This operation is important because it serves
     * to initialise the save game area, which is used to automatically
     * write out save files, without having to pass commands to the game
     * 
     * but the saves are encoded. that's another story...
     */

    static const char* setupCommands[] = 
    {
        // try loading a non-existent file
        "load",
        "dummyfile",
        "y",
    };
    
    for (size_t i = 0; i < DIM(setupCommands); ++i)
    {
        _evalCommand(setupCommands[i], cr);
        LOG4("MS, command: ", setupCommands[i] << ",  '" << cr->toString() << "'");
    }

    if (_ignoreOutput)
    {
        _ignoreOutput = false;
        
        // switch back transcript from NULL
        (_segmentInfoEmit)(_sictx, 0);  // to default
    }

    if (_undos.size())
    {
        autoLoad(0); // load latest, force "look".
    }

    delete cr;

    restartCommands(); // issue "verbose" etc.
}

void IFMagnetic::updateGameSaveArea(uchar* ptr, size_t size)
{
    if (_gameSaveMemory)
    {
        if (_gameSaveMemory != ptr || _gameSaveMemorySize != size)
        {
            LOG1("MS, save game memory area changed! ", ptr);
        }
    }
    
    _gameSaveMemory = ptr;
    _gameSaveMemorySize = size;

    LOG4("MS save area ", std::hex << (long)_gameSaveMemory << " size: " << _gameSaveMemorySize << std::dec);
}

bool IFMagnetic::saveGame(const char* name, uchar* ptr, size_t size)
{
    // remember the save game area
    updateGameSaveArea(ptr, size);
    
    SaveGameHeader shead;

    // mark type as saved by engine
    shead._type = SAVE_TYPE_ENGINE;

    return saveGame(name, shead);
}

bool IFMagnetic::saveGame(const char* name, SaveGameHeader& shead)
{
    bool res = false;
    if (name && *name)
    {
        assert(_gameSaveMemory);
        assert(_gameSaveMemorySize);

        string path = _undos.makeSavePath(name);
        
        FD file;
        res = file.open(path.c_str(), FD::fd_new);

        if (res)
        {
            shead.calcCrc(_gameSaveMemory, _gameSaveMemorySize);

            res = shead.write(file) &&
                file.write(_gameSaveMemory, _gameSaveMemorySize);
            
            if (res)
            {
                LOG2("MS, saved game '", path << "'");
            }
            else
            {
                LOG("MS, problem saving file '", path << "'");
            }
        }
        else
        {
            LOG("MS, can't open save file '", path << "'");
        }
    }
    return res;
}

bool IFMagnetic::loadGame(const char* name, uchar* ptr, size_t size)
{
    // remember the save game area
    updateGameSaveArea(ptr, size);

    SaveGameHeader shead;
    
    // mark as requiring a engine save
    shead._type = SAVE_TYPE_ENGINE;

    return loadGame(name, shead);
}

bool IFMagnetic::loadGame(const char* name, SaveGameHeader& shead)
{
    bool res = false;
    if (name && *name)
    {
        assert(_gameSaveMemory);
        assert(_gameSaveMemorySize);

        string path = _undos.makeSavePath(name);        

        FD file;
        res = file.open(path.c_str(), FD::fd_read);

        if (res)
        {
            SaveGameHeader ahead = shead;
            
            // first read header and skip over using its own length
            res = shead.read(file) && file.seek(shead._size);
            
            if (shead._magic != SAVE_MAGIC)
            {
                LOG("loadGame, not a magnetic save file! ", shead._magic);
            }

            if (res && ahead._type && ahead._type != shead._type)
            {
                // check header compat
                // if type specified, it must match
                LOG("loadGame, header type mismatch ", shead._type);
                res = false;
            }
            
            if (res) 
            {
                // read into temp memory, in case fail check
                uchar* data = new uchar[_gameSaveMemorySize];
                
                res = data != 0 && file.read(data, _gameSaveMemorySize);

                if (res && shead.versionOver(1,2))
                {
                    //LOG3("MS checking crc", "");
                
                    // header has crc, check it
                    res = shead.checkCrc(data, _gameSaveMemorySize);

                    if (!res) LOG("MS, ", "file crc error");
                }

                if (res)
                {
                    // only copy in loaded game if ok
                    memcpy(_gameSaveMemory, data, _gameSaveMemorySize);
                }
                
                delete [] data;
            }
         
            if (res)
            {
                LOG2("MS, loaded game '", path << "'");
            }
            else
            {
                LOG("MS, problem loading file '", path << "'");
            }
        }
        else
        {
            // doesn't have to exist
            LOG2("MS, can't open load file '", path << "'");
        }
    }
    return res;
}

void IFMagnetic::parseTranscript()
{
    Markup m;

    // automatically add markup to parts of the output text
    m.markupTextBuffer(_outbuf);
}

void IFMagnetic::tidyTranscript()
{
    // trim prompt and spaces on end
    
    char* p = _outbuf;
    char* q = _outbuf + strlen(_outbuf);

    for (;;)
    {
        // remove any spaces on end
        while (p != q && u_isspace(q[-1])) *--q = 0;

        // remove any prompt on end
        if (p != q && q[-1] == '>') *--q = 0;
        else break;
    }
}

bool IFMagnetic::setOptions(const VarSet& vs)
{
    for (VarSet::const_iterator it = vs.cbegin(); it != vs.cend(); ++it)
    {
        if (it->first == BRA_OPT_LOGLEVEL) setLogLevel(it->second.toInt());
        else if (it->first == BRA_OPT_MODERN)
        {
            _puzzlesEnabled = it->second.toInt() != 0;
            LOG3("MS, puzzles enabled ", _puzzlesEnabled);
        }
        else if (it->first == BRA_OPT_RANDOMSEED)
        {
            int64 seed = it->second.toInt();
            //LOG3("MS, random seed ", seed);
        }
        else if (it->first == BRA_OPT_PIXELSCALE)
        {
            bool v = it->second.isTrue();
            LOG3("MS use XBR pixel scaling ", (v ? "true" : "false"));
            if (v != _useXBR4)
            {
                _useXBR4 = v;

                // if we're not running then we're starting so this
                // is the same as last time
                if (_running)
                {
                    clearImageCache();

                    // reprocess current picture, if any
                    if (_lastPic.valid())
                    {
                        _showpic(&_lastPic);
                        flushImage();
                    }
                }
            }
        }
        else
        {
            LOG3("MS, ignoring option ", it->first);
        }
    }
    return true; // supported
}

void IFMagnetic::tidyStatusText()
{
#ifdef LOGGING
    if (Logged::_logLevel > 2)
    {
        int r = IItem::currentRoom().roomNumber();
        if (r)
        {
            char buf[16];
            sprintf(buf, "(%d) ", r);
                
            int sz = strlen(_statusBuf);
            int bz = strlen(buf);
            if (sz + bz < (int)sizeof(_statusBuf) - 1)
            {
                memmove(_statusBuf + bz, _statusBuf, sz + 1); // +terminator
                memcpy(_statusBuf, buf, bz);
            }
        }
    }
#endif
}

    
void ms_flush()
{
    ;
}

void ms_putchar(type8 c)
{
    theMS->emitchar((char)c);
}

type8 ms_getchar(type8 trans)
{
    return (type8)theMS->_getchar();
}

void ms_statuschar(type8 c)
{
    theMS->_putStatusChar(c);
}

void ms_showpic(type32 c,type8 mode, type8 ver, float* profile)
{
    IFMagnetic::PicRequest pr;
    pr._picn = c;
    pr._mode = mode;
    pr._ver = ver;
    pr._profile = profile;
    theMS->_showpic(&pr);
}

void ms_playmusic(type8 * midi_data, type32 length, type16 tempo)
{
    LOG2("MS ", "music");
}

type8 ms_save_file(type8s *name, type8 *ptr, type16 size)
{
    return theMS->saveGame((const char*)name, (uchar*)ptr, size) ? 0 : 1;
}

type8 ms_load_file(type8s *name, type8 *ptr, type16 size)
{
    return theMS->loadGame((const char*)name, (uchar*)ptr, size) ? 0 : 1;
}

void game_state_notify()
{
    // NB: on engine thread
    theMS->moveUpdate();
}

