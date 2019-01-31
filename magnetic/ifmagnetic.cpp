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

#include <sstream>
#include "ifmagnetic.h"
#include "strutils.h"
#include "qdefs.h"
#include "map.h"
#include "markup.h"
#include "varset.h"
#include "ifproduct.h"
#include "ifpic.h"
#include "filters.h"
#include "imgpng.h"
#include "rect.h"

#define VERSION_STRING  "9"

static IFMagnetic* theMS;

static int stop_restart_hook(int r, void* ctx, int request)
{
    // this hook is called twice, once before and once after
    // beforehand `request`==1 otherwise 0.

    if (request)
    {
        LOG3("MS ", (r ? "restart" : "stop") << " issued");
    }
    else
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
        _imageDir = makeDataPath("images");

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

    _initialising = true;

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

    // let the game start before issuing commands
    emitScene(); // sync, flush etc.

    // send initial setup commands to game
    initialCommands();

    // load puzzles after initial commands. this is to ensure game
    // has started. assume puzzles solving isn't needed by initial commands!
    _puzzles.start(this);

    // game ready to play
    _initialising = false;
    
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

    // reinstate classic/modern
    _puzzles.setRemaster(_modernMode);

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
                                std::placeholders::_1,
                                std::placeholders::_2);

    _messages.start(story);

    // set REMASTER here, *after* symbols loaded but *before* initial game code
    _puzzles.setRemaster(_modernMode);

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

void IFMagnetic::emitScene() 
{
    if (_sync())
    {
        bool r = flush();
        _syncLock.unlock();

        if (r)
        {
            updateAutoSave();
        }
    }
}

static void builderEmitter(char c, void* ctx)
{
    GrowString* buf = (GrowString*)ctx;
    buf->add(c);
}

void IFMagnetic::_buildSoundJSON(GrowString& buf, 
                                 const string& filepath,
                                 const SoundRequest* sr)
{
    // use builder helper, give it our emitter
    InfoBuilder build(builderEmitter, &buf);

    build.begin();
    build.tagString(BRA_SOUND_NAME, filepath);
    build.tagInt(BRA_SOUND_DURATION, sr->_duration);
    build.end();
    
    buf.add(0);  // terminate string
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
        build.picSaturation(profile[2]);
        build.picLightness(profile[3]);
        build.picGamma(profile[4]);
    }

    if (pr->_showInline) build.picInline(true);

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
                string imagepath = makeImagePath(files[i].c_str());
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

static bool scaleXBR(ImgData& src, ImgData& dst, int scaleFactor)
{
    int r = false;
    
    dst.format(src._width*scaleFactor, src._height*scaleFactor, src._bpp);
    dst.create();
                    
    xbr_params xbrParams;
    xbrParams.input = src._data;
    xbrParams.output = dst._data;
    xbrParams.inWidth = src._width;
    xbrParams.inHeight = src._height;
    xbrParams.inPitch = src._stride;
    xbrParams.outPitch = dst._stride;

    switch (scaleFactor)
    {
    case 2: xbr_filter_xbr2x(&xbrParams); r = true; break;
    case 3: xbr_filter_xbr3x(&xbrParams); r = true; break;
    case 4: xbr_filter_xbr4x(&xbrParams); r = true; break;
    }

    return r;
}

static bool makeMSImage(ImgData& img, int w, int h, ushort* pal,
                        uchar* data, uchar* mask)
{
    // sanity
    bool r = w > 0 && w < 1024 && h > 0 && h < 1024;
    
    if (r)
    {
        img.format(w, h, 32);
        img.create();

        // mask stride
        int mstride = ((w + 15)/16)*2;

        for (int y = 0; y < h; ++y)
        {
            uchar* s = data + y*w; // source stride 1
            uchar* q = img._data + img._stride*y;
            uchar* mp = mask ? mask + y*mstride : 0;
                    
            for (int x = 0; x < w; ++x)
            {
                uint v = (*s++) & 0xf;
                v = pal[v];
                
                uint alpha = 0xff;

                if (mp)
                {
                    int q = x/8;
                    int r = 7 - (x & 7);
                    if (mp[q] & (1<<r)) alpha = 0;
                }
                
                *q++ = (v&0x0F00)>>3; // red
                *q++  = (v&0x00F0)<<1; // green
                *q++ = (v&0x000F)<<5; // blue
                *q++ = alpha;
            }
        }
    }

    if (!r)
    {
        LOG1("MS,", "failed to extract MS image");
    }
    
    return r;
}

static bool makeMSImageScaled(ImgData& img, int w, int h, ushort* pal,
                              uchar* data, uchar* mask, int scaleFactor,
                              float* pcp)
{
    bool r = makeMSImage(img, w, h, pal, data, mask);

    if (pcp)
    {
        // optional cropping
        int ctop = pcp[5];
        int cright = pcp[6];
        int cbottom = pcp[7];
        int cleft = pcp[8];

        if (ctop || cright || cbottom || cleft)
        {
            img.crop(ctop, cright, cbottom, cleft);
        }
    }
    
    if (r && scaleFactor > 1)
    {
        // apply XBR scale
        ImgData out;
        r = scaleXBR(img, out, scaleFactor);
        if (r) img = out; // donate
    }
    return r;
}

std::string IFMagnetic::_decodePicture(const PicRequest* pr,
                                      const char* imagename)
{
    // decode picture and save it
    // return the full path of the picture saved (with suffix)
    
    type16 w, h;
    type16 pal[16];
    type8 isAnim;

    string savedFilename;

    // full path without suffix
    string imagepath = makeImagePath(imagename);

    int code = pr->_ver > 1 ? pr->_picAddr : pr->_picn;
    uchar* p = ms_extract(code, &w, &h, pal, &isAnim);
    if (p)
    {
        LOG4("MS ", "Have animation? " << (int)isAnim);

        ImgData bgimg;
        if (makeMSImageScaled(bgimg, w, h, pal, p, 0, (_useXBR4 ? 4 : 1),
                              pr->_profile))
        {
            LOG3("MS showpic, saving ", imagepath);
                                
            // write directly with libpng
            ImgAPng png(&bgimg);
                
            if (isAnim)
            {
                struct ms_position * positions;
                type16 count, width, height;
                type8 * pmask;
                int cc = 0;
                Rect rframeLast;

                while (ms_animate(&positions,&count) != 0)
                {
                    ImgData playback;
                    playback.copy(bgimg);
                    Rect rframe;

                    // render images for this frame
                    for (int i = 0; i < count; i++)
                    {
                        type8* picdata = ms_get_anim_frame(positions[i].number,&width,&height,&pmask);

                        if (picdata)
                        {
                            LOG4("Extracting anim ","frame: " << positions[i].number << " x: " << positions[i].x << " y: " << positions[i].y << " width: " << width << " height: " << height);

                            ImgData img;
                            int scale = _useXBR4 ? 4 : 1;
                            if (makeMSImageScaled(img, width, height,
                                                  pal, picdata, pmask,
                                                  scale,0))
                            {
                                ++cc;

                                int px = positions[i].x*scale;
                                int py = positions[i].y*scale;

                                // clip against background 
                                img.clip(px, py, bgimg._width, bgimg._height);

                                Rect ri(px, py, img._width, img._height);
                                rframe = rframe.combine(ri);

                                // blit onto background
                                if (pmask)
                                {
                                    // has mask data
                                    playback.blitAlpha(img, 0, 0,
                                                       px, py,
                                                       img._width, img._height);
                                }
                                else
                                {
                                    // otherwise just copy over
                                    playback.blit(img, 0, 0,
                                                  px, py,
                                                  img._width, img._height);
                                }
                            }
                        }
                    }

                    // combine last & current areas
                    Rect rchange = rframe.combine(rframeLast);

                    // make a frame for the combined change
                    ImgData cimg;
                    cimg.format(rchange._w, rchange._h, playback._bpp);
                    cimg.create();
                    
                    cimg.blit(playback, rchange._x, rchange._y,
                              0, 0,
                              rchange._w, rchange._h);

                    png.addFrame(cimg, rchange._x, rchange._y);
                    
                    rframeLast = rframe;
                }

                LOG3("animated image ", imagepath << " total " << cc << " frames");

            }

            png.write(imagepath);

            // APNG or PNG 
            savedFilename = png._filename;
        }
    }
    else
    {
        LOG1("MS could not find picture ", imagename);
    }

    return savedFilename; // null if not saved
}

void IFMagnetic::showImage(const string& filepath, const PicRequest* pr)
{
    if (!filepath.empty())
    {
        // build picture json
        GrowString buf;
        _buildPictureJSON(buf, filepath, pr);

        // should not be one, but just incase
        _dropImage();
        
        // takes out the string and gives it to us
        _imageJSONBuf = buf.donate();
    }
}

void IFMagnetic::_showNewVersionPic(PicRequest* pr)
{
    if (!pr->_picAddr) return; // bail
    
    const char* p = (const char*)getcode() + pr->_picAddr;
    
    LOG3("MS show new pic name: ", p);

    // will be resolved by QControl::resolveAsset, which will look first
    /// in Qt resources, then in configdir/
    string file = "images/" + toLower(p);

    // dont apply any profile to new version pictures
    pr->_profile = 0;

    LOG3("show new version pic '", file << "'");
    showImage(file, pr);
}

void IFMagnetic::_playSoundJSON(GrowString& buf)
{
    LOG3("MS, playsoundJSON ", buf.start());
    
    // in case old still here
    _dropSound();

    // takes out the string and gives it to us
    _soundJSONBuf = buf.donate();
}

void IFMagnetic::_playSound(SoundRequest* sr)
{
    GrowString buf;
        
    if (sr->_soundFile)
    {
        LOG3("MS, playsound ", sr->_soundFile << " in " << sr->_room);

        // will be resolved by QControl::resolveAsset, which will look first
        /// in Qt resources, then in configdir/
        string file = "sounds/" + toLower(sr->_soundFile);

        // build sound json
        _buildSoundJSON(buf, file, sr);
    }
    else
    {
        //LOG3("MS, ", "stopsound");
        // empty json means stop playing sound
        buf.append("{}");
        buf.add(0);
    }

    _playSoundJSON(buf);

}

void IFMagnetic::_showpic(PicRequest* pr)
{
    LOG3("MS showpic #", pr->_picn << " mode " << pr->_mode << " ver " << pr->_picVer);

    if (pr->_mode == 0)
    {
        LOG3("MS hidepic ", pr->_picn);
        clearImage();
        return;
    }

    if (pr->_picVer > 0)
    {
        _showNewVersionPic(pr);
        return;
    }
    
    char imagename[128];

    // prefix
    strcpy(imagename, "ms_");
    
    if (pr->_picAddr)
    {
        const char* p = (const char*)getcode() + pr->_picAddr;
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

    // don't ignore picture for ignoroutput;
    // will load start picture then either
    // (a) will be at start
    // (b) will load a save for a location that will load another picture
    // (c) will load a save for a location with no picture BUT now a hide is issued for those locations.
    /*
    if (_ignoreOutput)
    {
        LOG3("ignoring picture ", imagename);
        return;
    }
    */

    if (_useXBR4)
    {
        // use a different name for processed images
        strcat(imagename, "_xbr4");
    }
    
    _lastPic = *pr;

    // exists or not
    string imagepath = makeImagePath(imagename);
    string savedFilename;

    // first look for PNG
    savedFilename = changeSuffix(imagepath, ".png");

    bool found = FD::exists(savedFilename.c_str());
    if (!found)
    {
        // try APNG
        savedFilename = changeSuffix(imagepath, ".apng");
        found = FD::exists(savedFilename.c_str());
    }

    if (!found)
    {
        // decode it
        savedFilename = _decodePicture(pr, imagename);
        found = !savedFilename.empty();
    }

    if (found)
        showImage(savedFilename, pr);
}

std::string IFMagnetic::prettyCommand(const char* cmd)
{
    // neaten the command for display.
    string s;
    if (cmd && *cmd)
    {
        s += u_toupper(*cmd);
        s += toLower(cmd + 1);

        char last = cmd[strlen(cmd)-1];
        if (!strchr(".?!\"", last))
            s += '.';
    }
    return s;
    
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
            _emits(_consoleEmit, _cctx, prettyCommand(cmd).c_str());

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
                word = ii.adjWordIf();
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

bool IFMagnetic::undo(int delta)
{
    bool v = autoLoad(delta, true);
    LOG3("Undo: ", _undos);
    return v;
}

bool IFMagnetic::_handleTestCommands(const char* cmd)
{
    bool handled = false;
    
    bool enabled = false;
    if (*cmd == '#') enabled = true;
        
    // accept $ as prefix so that it can be in script files where # is comment
    // keep in release, so we can test!
    if (*cmd == '$') enabled = true;
    
    if (enabled)
    {
        // this goes into release builds now so we can have secret
        // commands to make screenshots etc.
        
        extern int handle_test_commands(const char* cmd);

        // capture cout and send to UI
        std::stringstream buffer;
        std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

        if ((handled = !strcmp(cmd+1, "showmap")) != 0)
        {
            _allMapCheat = true;
            std::cout << "Map Shown\n";
        }
        else if ((handled = (cmd[1] == '(')) != 0)
        {
            _puzzles.evalKL(cmd + 1, true);
        }

        if (!handled) handle_test_commands(cmd + 1);
        std::cout.rdbuf(old);
        LOG3("test command ", cmd << ": " << buffer.str());
        handled = true;

    }
    return handled;
}

bool IFMagnetic::_evalCommandSpecial(const char* cmd, CommandResultI* cres)
{
    // handle special cases
    assert(cmd);

    CommandResult* cr = (CommandResult*)cres;

    // handle undo/redo first. other commands reset the undo state
    if (!u_stricmp(cmd, "undo"))
    {
        bool v = autoLoad(-1, false);        
        LOG3("Undo: ", _undos);
        if (cr)
            cr->_result = v ? "undo " + std::to_string(_undos.position()) : "no more undo";
        
        return true;
    }
    else if (!u_stricmp(cmd, "redo"))
    {
        bool v = autoLoad(1, false);
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
    else if (!u_stricmp(cmd, "resetgame"))
    {
        restartGame();

        return true;
    }

    // deal with special debug commands
    if (_handleTestCommands(cmd)) return true;
    
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
            IItem::IItems items;

            // without "do"
            string phrase = unsplit(words, ' ', 1, n);

            // check for phrase match
            IItem::phraseResolve(items, phrase);
            
            if (items.size() == 1)
                res = puzzleForX(0, 0, phrase, act);
            else
            //if (!res)
            {
                const string* adj = 0;
                const string* adj2 = 0;
                if (n > 3) adj2 = &words[n-3];
                if (n > 2) adj = &words[n-2];

                // then look for a puzzle bound to the root word (if enabled)
                res = puzzleForX(adj2, adj, words[n-1], act);
            }

            if (!res)
            {
                // otherwise useX
                res = evalUseX(phrase, false);
            }
        }
    }
    else if (equalsIgnoreCase(words[0], "domenu"))
    {
        res = evalUseX(unsplit(words, ' ', 1, n), true);
    }
    else if (equalsIgnoreCase(words[0], "_goto")) 
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
    else if (n == 2 && equalsIgnoreCase(words[0], "script"))
    {
        string spath = makeConfigPath(words[1].c_str());
        res = true;

        FD fd;
        if (fd.open(spath.c_str()))
        {
            // read all the script text, converting from DOS (where present)
            uchar* stext = fd.readAll(0, true);
            if (stext)
            {
                char* s = (char*)stext;
                char* p;

                for (;;s = p)
                {
                    while (u_isspace(*s)) ++s;
                    if (!*s) break; // done
                    
                    p = s; 
                    while (*p && *p != '\n') ++p;  // end of line

                    // skip blank lines and comments (# lines)
                    if (*s == '#' || p == s) continue;
                    
                    string cmd(s, p - s);

                    //echo
                    res = evalCommand(cmd.c_str(), 0, true);
                    if (!res) break;

                    emitScene();
                }
                
                delete [] stext;
            }
            else
            {
                LOG1("Problem reading script file '", spath << "'");
            }
        }
        else
        {
            LOG1("Script file '", spath << "' not found");
        }
    }
    return res;
}

bool IFMagnetic::puzzleForX(const string *adj2, const string* adj, const string& x, int act)
{
    // look for a puzzle for x
    // if adj and no puzzle for x, look for a puzzle for "adj x".
    // and if adj2 and no puzzle for adj x, look for "adj2 adj x"
    
    bool res = false;
    Puzzle* p = 0;

    if (!_puzzles._enabled) return false; // switched off

    // prefer adj+adj+word
    if (adj2 && adj)
    {
        string ax = *adj2 + ' ' + *adj + ' ' + x;
        p = _puzzles.find(ax.c_str());
    }
    // prefer adjective+word
    if (!p && adj)
    {
        string ax = *adj + ' ' + x;
        p = _puzzles.find(ax.c_str());
    }
    
    if (!p)
    {
        // try plain x
        p = _puzzles.find(x.c_str());
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
            
            if ((xi.gettable() && !xi.carried())||(xi.carried() && !xi.simplyCarried()))
            {
                if (_puzzles.allowSuggestGet(xi)) seq.add("get");
            }
            if (xi.isClosed() || xi.isLocked())
            {
                // puzzle code can allow/disallow open in various cases
                if (_puzzles.allowSuggestOpen(xi)) seq.add("open");
            }
            if (xi.isWearable() && !xi.isWorn())
            {
                seq.add("wear");
            }
            if (xi.isContainer() && !xi.isClosed() && !xi.isLocalContents())
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
            if (xi.isBigThing())
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

        bool done = false;
        cmd = _puzzles.evalUseXwithYSpecial(xi, yi, done); // &done

        if (done) return true; // handled

        if (cmd.empty())
        {
            if (yi.isNPC())
            {
                if (xi.isNPC() || !xi.couldGet())
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

#if !defined(NDEBUG)
    // level 2 and above, see whole map
    if (Logged::_logLevel >= 2) _allMapCheat = true;
#endif
    
    for (size_t i = 0; i < gm.size(); ++i)
    {
        // each room PPS gives a grid position
        const MapPos& mpi = gm.at(i);

        IItem ri = IItem::getRoom(mpi.room);

        // collect only rooms we know about
        if (ri && (ri.visibleOnMap() || _allMapCheat))
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
        string name = ri.toString();

#if !defined(NDEBUG)
        // level 3 and above, show room numbers
        if (Logged::_logLevel >= 3)
        {
            char buf[32];
            sprintf(buf, " (%d)", mpi.room);
            //sprintf(buf, " (%x)", mpi.room);
            name += buf;
        }
#endif        

        build.beginPlace();
        build.placeID(mpi.room);
        build.placeName(name);
        build.placeX(mpi.pos.x);
        build.placeY(mpi.pos.y);

        // leave out if lit (is default)
        if (!ri.isLit()) build.placeDark(true);

        if (ri.getExit(IItem::dir_u)) build.placeIndicatorUp(true);
        if (ri.getExit(IItem::dir_d)) build.placeIndicatorDown(true);
        
        // to indicate items of interest in room, accept only
        // explored, carryable and non-live NPC.
        IItem::IItems roomItems;
        ri.getItemsInRoom(roomItems, true, true, true);
        build.placeItemCount(roomItems.size());

        // rooms may be visible on the map even though they aren't explored.
        // ie, they have never been visible.
        // rooms that are visible but not explored do not show map links
        if (ri.isExplored() || _allMapCheat)
        {
            // exits
            int exits[ITEM_MAX_EXITS];
            int ne = ri.getExitRooms(exits);

            if (ne > 0)
            {
                build.beginExits();
                for (int i = 0; i < ne; ++i)
                {
                    IItem rj = IItem::getRoom(exits[i]);
                    if (rj.isExplored() || _allMapCheat)
                        build.exit(exits[i]);
                }
                build.endExits();
            }
        }
        
        build.endPlace();
    }

    build.endPlaces();
    build.end();
    buf.add(0);  // terminate string
}

bool IFMagnetic::updateMapInfo(MapInfo& mi)
{
    IItem croom = IItem::currentRoom();

    mi._currentLocation = std::to_string(croom.roomNumber());
    mi._currentExits = 0;

    // `currentExits` is a bit mask of exits 
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

    LOG3("MS ", "updateMapInfo " << (mi._changed ? "changed" : "not changed"));

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
    GrowString buf;
    _buildRosterJSON(buf);

    // LOG3("MS, updateRosterInfo ", buf.start());
    
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

void IFMagnetic::moveUpdate(int moveCount)
{
    // this gets called every game move.
    // NOTE: on game thread

    // for mag binaries (eg Pawn), we're called according to save_undo
    // triggered from undo_pc.
    // but for prog binaries, we don't have undo_pc NOR do we have emu
    // undos. So instead we're called from trap A0E3 (make_undo)
    //
    // NO! DO not autosave here!!
    //updateAutoSave();

    _puzzles.moveUpdate(moveCount);
}

bool IFMagnetic::updateAutoSave()
{
    // NB: on engine thread
    bool res = _gameSaveMemory != 0 && _undos.canAutoSave() && !_initialising;

    if (res)
    {
        int v = get_cantsave();
        if (v)
        {
            LOG3("MS, save suspended by game ", v);
            res = false;
        }
    }
    
    if (res)
    {
        // next save name
        string sname = _undos.autosaveName(_undos._last + 1);
        
        // raw save game file UI format
        res = _saveGame(sname.c_str(), SAVE_TYPE_MEMIMAGE, _saveContext);

        if (res) _undos.advanceLast(); 

    }
    return res;
}

bool IFMagnetic::loadMemGame(const char* name,
                             SaveGameHeader& shead,
                             SaveContext& sctx)
{
    bool res = loadGame(name, shead, sctx);

    if (res && sctx._clearPic)
    {
        // clear any old pic before look
        clearImage();

        // reset the game's idea of the current picture so that
        // after a load, the "look" will cause the picture to be loaded
        // in the case where we're loading a game in the current location
        set_PICTNUM(0);
    }

    if ((res || sctx._forceLook) && !sctx._neverLook)
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

bool IFMagnetic::autoLoad(int delta, bool neverLook)
{
    bool res = false;
    
    if (!delta || _undos.deltaCurrent(delta))
    {
        // NB: if load fails (eg corrupt file) continue to force a look
        // otherwise get blank screen.

        // BUT if neverLook, will will never look regardless!
        // what is this?
        //
        // Currently we have a situation were some loads are requested
        // through the UI (including undo/redo) in these cases
        // we will force a look because otherwise the game text is not
        // updated
        // but SOMETIMES the load is via the game (eg DO.death -> undo)
        // in _these_ cases, the game itself will refresh the text.
        //
        // eventually, we'll add undo/redo support to the game and then
        // the UI version can invoke the game version, then we
        // wont have to force look (nor will load/save cost a move!)

        _saveContext._clearPic = false;
        _saveContext._forceLook = true;
        _saveContext._neverLook = neverLook;
        res = _loadGame(_undos.autosaveName(0).c_str(), SAVE_TYPE_MEMIMAGE, _saveContext);
    }
    
    return res;
}

void IFMagnetic::restartCommands()
{
    // issues commands to the game after restarted

    // ensure we're in "verbose" mode.

    // prog_mode can set internal state directly, if not, we have to issue
    // game command manually
    bool v = set_OUTMODE(1) != 0;

    if (v)
    {
        LOG3("MS, ", "enabled verbose mode directly");
    }

    if (!v)
    {
        UndoState::SuspendSave ss(_undos);
    
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
}

void IFMagnetic::initialCommands()
{
    // issue commands to the game on first start
    
    // No need for dummy load if we already have the save game area
    // (ie prog_format)
    if (!_gameSaveMemory)
    {
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

        delete cr;
    }


    if (_ignoreOutput)
    {
        _ignoreOutput = false;
        
        // switch back transcript from NULL
        (_segmentInfoEmit)(_sictx, 0);  // to default
    }

    if (_undos.size())
    {
        //LOG3("MS, ", "restoring previous game");
        autoLoad(0, false); // load latest, force "look".
    }

    restartCommands(); // issue "verbose" etc.
}

void IFMagnetic::updateGameSaveArea(uchar* ptr, size_t size, uint addr)
{
    assert((addr & 0xffff) == addr);
    
    if (_gameSaveMemory)
    {
        if (_gameSaveMemory != ptr ||
            _gameSaveMemorySize != size ||
            _gameSaveAddr != addr)
        {
            LOG1("MS WARNING: ", "save game memory area changed!");
        }
    }
    
    _gameSaveMemory = ptr;
    _gameSaveMemorySize = size;
    _gameSaveAddr = addr;

    LOG4("MS save area ", std::hex << _gameSaveAddr << " size: 0x" << _gameSaveMemorySize << std::dec);
}

bool IFMagnetic::saveGame(const char* name,
                          SaveGameHeader& shead, SaveContext& sctx)
{
    bool res = false;
    if (name && *name)
    {
        assert(sctx._ptr);
        assert(sctx._size);
        assert(sctx._addr);

        string path = _undos.makeSavePath(name);

        shead._gameid = get_game();

        const uchar* xdata = 0;
        size_t xsize = 0;

        // do we have any extra data to save?
        if (_requestSaveExtraFn)
        {
            if ((*_requestSaveExtraFn)(_requestSaveExtraCtx, &xdata, &xsize) &&
                xdata && xsize > 0)
            {
                // have any extra data to save?
                shead._extraDataSize = xsize;
            }
        }

        
        FD file;
        res = file.open(path.c_str(), FD::fd_new);

        if (res)
        {
            shead.calcCrc(sctx._ptr, sctx._size);
            shead._dataSize = sctx._size;
            shead._dataAddr = sctx._addr;

            // save header and game data
            res = shead.write(file) && file.write(sctx._ptr, sctx._size);

            if (res && shead._extraDataSize)
                res = file.write(xdata, xsize);
            
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

bool IFMagnetic::loadGame(const char* name,
                          SaveGameHeader& shead,
                          SaveContext& sctx)
{
    bool res = false;
    if (name && *name)
    {

        assert(sctx._ptr);
        assert(sctx._size);
        assert(sctx._addr);

        string path = _undos.makeSavePath(name);        

        FD file;
        res = file.open(path.c_str(), FD::fd_read);

        if (res)
        {
            SaveGameHeader ahead = shead;
            
            // first read header and skip over using its own length
            res = shead.read(file) && file.seek(shead._size);
            
            if (shead._magic != SAVE_MAGIC)
                LOG("loadGame, not a magnetic save file! ", shead._magic);

            if (res && ahead._type && ahead._type != shead._type)
            {
                // check header compat
                // if type specified, it must match
                LOG("loadGame, header type mismatch ", shead._type);

                // allow mismatch for prog format, as they should be compatible
                if (!prog_format) res = false;
            }
            
            if (res) 
            {
                if (shead.oldVersion())
                {
                    LOG1("MS loading old save game version ", shead._versionMajor << "." << shead._versionMinor);
                }
                
                // read into temp memory, in case fail check
                uchar* data = new uchar[sctx._size];
                
                res = data != 0 && file.read(data, sctx._size);
                
                // support game_id
                if (res && shead.versionOver(1,5))
                {
                    if (shead._gameid != get_game())
                    {
                        LOG1("MS, load game, wrong game ", shead._gameid);
                        res = false;
                    }
                    else
                    {
                        LOG3("MS, load game gameid OK ", shead._gameid);
                    }
                }
                else
                {
                    shead._gameid = 0; // unknown
                }

                if (res && shead.versionOver(1,3))
                {
                    //LOG3("MS checking save size ", sctx._size);
                    res = shead._dataSize == sctx._size;
                    if (!res)
                    {
                        LOG1("MS, Save Game Size Mismatch; expected ", sctx._size << " got " << shead._dataSize);                        
                    }
                    
                    if (shead._dataAddr != sctx._addr)
                    {
                        LOG1("MS, Save Game Addrees Mismatch; expected ", sctx._addr << " got " << shead._dataAddr);                        
                        res = false;
                    }
                }
                else
                {
                    // not supported
                    shead._dataSize = 0;
                    shead._dataAddr = 0;
                }

                if (res && shead.versionOver(1,2))
                {
                    //LOG3("MS checking crc", "");
                
                    // header has crc, check it
                    res = shead.checkCrc(data, sctx._size);

                    if (!res) LOG("MS, file crc error, expected ", std::hex << shead._crc32 << " got " << crc32(data, sctx._size) << " from " << std::dec << sctx._size << " bytes");
                }

                // support extra data?
                if (res && shead.versionOver(1,4))
                {
                    // drop any previous loaded extra data
                    sctx.dropLoad();

                    size_t xsize = shead._extraDataSize;
                    
                    // put a load limit, in case file corrupt
                    if (xsize > 1024*1024)
                    {
                        LOG1("MS, load extra data too large ", shead._extraDataSize);
                    }
                    else if (xsize > 0 && _requestLoadExtraFn)
                    {
                        //LOG3("load game has extra data, size: ", shead._extraDataSize);

                        uchar* xdata = new uchar[xsize];
                        assert(xdata);

                        // if, for some reason, the extra data fails to 
                        // load then this is not fatal for a load game.
                        
                        bool v = file.read(xdata, xsize);
                        if (v)
                        {
                            // give data to context
                            sctx.setLoad(xdata, xsize);

                            // anyone want this data??
                            (*_requestLoadExtraFn)(_requestLoadExtraCtx,
                                                   (const uchar**)&xdata,
                                                   &xsize);
                        }
                        else
                        {
                            LOG1("MS, WARNING: extra data failed to load ", shead._extraDataSize);
                            delete [] xdata;
                        }
                    }
                }
                else shead._extraDataSize = 0;

                if (res)
                {
                    // only copy in loaded game if ok
                    memcpy(sctx._ptr, data, sctx._size);
                }
                
                delete [] data;
            }
         
            if (res)
            {
                // puzzle assistant state not saved, so reset on load
                _puzzles.reset();
                
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
        if (p == q || q[-1] != '>') break;

        *--q = 0;

        // remove "-" before prompt (eg --->)
        //while (p != q && q[-1] == '-') *--q = 0;
    }
}

bool IFMagnetic::setOptions(const VarSet& vs)
{
    for (VarSet::const_iterator it = vs.cbegin(); it != vs.cend(); ++it)
    {
        if (it->first == BRA_OPT_LOGLEVEL) setLogLevel(it->second.toInt());
        else if (it->first == BRA_OPT_MODERN)
        {
            bool v = it->second.toInt() != 0;
            
            _puzzles.enabled(v);

            // set here *before* game starts, later used in `start`
            _modernMode = v;

            if (_running)
            {
                // if running set right away, so changes mid-game work
                _puzzles.setRemaster(_modernMode);
            }

        }
        else if (it->first == BRA_OPT_RANDOMSEED)
        {
            // we're pass a random seed from the front end, if we need it
            //int64 seed = it->second.toInt();
            //LOG3("MS, random seed ", seed);
        }
        else if (it->first == BRA_OPT_PIXELSCALE)
        {
            bool v = it->second.isTrue();
            LOG4("MS use XBR pixel scaling ", (v ? "true" : "false"));
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

bool IFMagnetic::restartGame() 
{
    LOG3("MS, ", "restartGame");

    if (!_sync()) return false;

    // initiate restart in emu. 
    // will callback through hook to `gameRestarting`
    ms_restart_request();

    _inpos = 0;
    _inbuf[0] = 1;  // continue signal
    _inbuf[1] = 0;

    _inputReady = true;
    _waiter.notify_one();
    _syncLock.unlock();

    emitScene();

    // then issue "verbose" and any others
    restartCommands();

    return true;
}

// ----------------------------------------
    
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

void ms_showpic(type32 c, type32 picAddr, type8 mode, type8 ver, float* profile, int picVer)
{
    IFMagnetic::PicRequest pr;
    pr._picn = c;
    pr._picAddr = picAddr;
    pr._mode = mode;
    pr._ver = ver;
    pr._profile = profile;
    pr._picVer = picVer;
    theMS->_showpic(&pr);
}

void ms_playsound(int room, const char* name)
{
    IFMagnetic::SoundRequest sr;
    sr._room = room;
    sr._soundFile = name;
    theMS->_playSound(&sr);
}

void ms_playmusic(type8 * midi_data, type32 length, type16 tempo)
{
    LOG2("MS ", "music");
}

type8 ms_save_file(type8s *name, type8 *ptr, type16 size, unsigned int addr)
{
    theMS->updateGameSaveArea(ptr, size, addr);
    return theMS->saveGameEMU((const char*)name) ? 0 : 1;
}

type8 ms_load_file(type8s *name, type8 *ptr, type16 size, unsigned int addr)
{
    theMS->updateGameSaveArea(ptr, size, addr);
    return theMS->loadGameEMU((const char*)name) ? 0 : 1;
}

void game_state_notify(int movecount)
{
    // NB: on engine thread
    theMS->moveUpdate(movecount);
}

void update_game_save_area(uchar* ptr, size_t size, unsigned int addr)
{
    // can some straight from emu
    theMS->updateGameSaveArea(ptr, size, addr);
}

void ms_undo_signal()
{
    // from game trap A0E2
    LOG3("MS, ", "undo signal");
    theMS->undo(0);  // restore game delta 0 (ie latest)
}

void ms_event_hook(int quiet)
{
    // called after location description printed and nothing is happening
    theMS->_handleEvent(quiet);
}

void ifiSend(const char* js)
{
    // hook for IFI-style responses within KLMAG
    GrowString buf;
    buf.append(js);
    buf.add(0);
    theMS->_playSoundJSON(buf);
}

