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
#include <QDir>
#include "ifglk.h"
#include "strutils.h"
#include "qdefs.h"
#include "varset.h"
#include "ifproduct.h"

extern "C"
{
#include "glk.h"    
};

static IFGLK* theMS;

struct glk_window_struct: public IFGLK::Window
{
    winid_t     _split;
    glui32      _method;
    glui32      _size;
    glui32      _type;
    glui32      _rock;
};

bool IFGLK::start(const char* configDir,
                  const char* dataDir,
                  const char* story,
                  charEmitFn* transcriptEmit, void* tctx,
                  charEmitFn* consoleEmit, void* cctx,
                  segmentInfoFn* sfn, void* sctx)
{
    _story = story;

    _configDir = configDir;
    _dataDir = dataDir;

    LOG3("GLK, configDir ", _configDir);
    LOG3("GLK, dataDir ", _dataDir);
    LOG3("GLK, story ", _story);
    
    if (_createImageDir)
    {
        const char* imagesdir = "images";
        
        // images stored in a subdirectory
        QDir sdir(dataDir);
        if (!sdir.exists(imagesdir)) sdir.mkdir(imagesdir);
        sdir.cd(imagesdir);
        _imageDir = STRQ(sdir.path());

        LOG3("GLK, imageDir ", _imageDir);
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
    
    _msTask = std::thread(&IFGLK::runMsTask, this);
    return true;
}

void IFGLK::runMsTask()
{
    _running = true;
    LOG3("GLK, ", "running");    

   if (!setjmp(_jmpExit))
   {
       glk_main();
       LOG1("GLK, ", "failed to load story " << _story);
   }

   _running = false;

   LOG3("MS ", "shutdown");    
}

void IFGLK::_showpic(int picn, int mode)
{
    LOG1("MS showpic #", picn << " mode " << mode);

    QDir idir(_imageDir.c_str());
    char imagename[64];
    sprintf(imagename, "image%d.png", picn);

    // exists or not
    string imagepath = STRQ(idir.filePath(imagename));

    if (!idir.exists(imagename))
    {
        // decode picture and save it

        LOG3("MS showpic, saving ", imagepath);
                
        //img->save(imagepath.c_str(), "PNG");
        //delete img;

    }
    
    // post for display
    strcat(_imageNameBuf, imagename);
}

bool IFGLK::_evalCommand(const char* cmd, CommandResultI* res)
{
    // NB: ONLY call after sync (assumes lock)
    
    LOG3("ms, eval command \"", cmd << "\"");

    if (_inbufsz > 0)
    {
        strncpy(_inbuf, cmd, _inbufsz-1);
        _inbuf[_inbufsz-1] = 0; // ensure termination

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

static bool isThing(const char* s)
{
    static const char* dets[] =
    {
        "a ",
        "an ",
        "the ",
        "some ",
    };

    for (size_t i = 0; i < DIM(dets); ++i)
        if (startsWith(s, dets[i])) return true;
    return false;
}

void IFGLK::bogusInventory(const char* buf, CommandResult* cres)
{
    // parse output of a command to try to figure the inventory.
    // not the way it should work, but ...
    
    const char* p = strchr(buf, ':'); // ignore up to colon if exists
    if (!p) p = strchr(buf, ';'); // or semicolon
    if (!p) p = buf;
    else ++p;

    // split up the strings by comma
    std::vector<std::string> items;
    split(items, p, ',');
    for (size_t i = 0; i < items.size(); ++i)
    {
        string si = trim(items[i]);

        // look at each one and see if if looks like a "thing"

        //LOG3("glk inv ", i << " " << si);

        if (si == "Nothing") return; // bail

        if (isThing(si.c_str()))
        {
            if (!cres->_result.empty()) cres->_result += ',';
            cres->_result += si;
        }
    }
}

bool IFGLK::evalCommandSpecial(const char* cmd, CommandResultI* cres)
{
    // handle special cases
    bool res = false;
        
    if (cres)
    {
        CommandResult* cr = (CommandResult*)cres; 
        if (!strcmp(cmd, "eval game"))
        {
            // XX extract name of game from engine
            cr->_result = "XVAN";
            res = true;
        }
        if (!strcmp(cmd, "eval all in me"))
        {
            if (_invWindow)
            {
                _sync();
                //LOG3("glk, ", "eval special");
                _invWindow->clear();

                _outputWindow->_redirect = _invWindow;
                
                _evalCommand("i", cres);

                // process & wait
                _sync();

                // revert window
                _outputWindow->_redirect = 0;

                // all good
                _syncLock.unlock();

                _invWindow->tidyTranscript();
                
                LOG3("glk inv, ", _invWindow->_buf);

                bogusInventory(_invWindow->_buf, cr);
                
            }
            res = true;
        }
    }
    return res;
}

bool IFGLK::setOptions(const VarSet& vs)
{
    for (VarSet::const_iterator it = vs.cbegin(); it != vs.cend(); ++it)
    {
        if (it->first == BRA_OPT_LOGLEVEL) setLogLevel(it->second.toInt());
    }
    return true; // supported
}

static void builderEmitter(char c, void* ctx)
{
    GrowString* buf = (GrowString*)ctx;
    buf->add(c);
}

void IFGLK::_buildProductInfoJSON(GrowString& buf)
{
    // use builder helper, give it our emitter
    ProductInfoBuilder build(builderEmitter, &buf);

    build.beginProduct();

    build.productTitle("The Game");
    build.productAuthor("by XVAN");

    GrowString gs;
    const char* c = 
                "<h1>Credits</h1>"
        
                "<h4>Strand Games Team</h4>"
                "<ul>"
                "<li>Hugh Steers</li>"
                "<li>Stefan Meier</li>"
                "<li>Rob Steggles</li>"
                "</ul>"
        
                "<h4>Art</h4>"

                "<ul>"
                "<li>None</li>"
                "</ul>"

                "<h4>Music</h4>"
                "<ul>"
                "<li>None</li>"
                "</ul>"
        
                "<h4>Testing</h4>"
                "<ul>"
                "<li>None</li>"
                "</ul>";

    gs.append(c);
    gs.add(0);
    
    build.productCredits(gs.start());

    // add theme colours
    build.productThemePrimeColor("#ff5722");
    build.productThemeAccentColor("red");
    build.productThemeTabColor("white");
    build.productThemeContrastColor("#003ed8");
    
    // cover page
    build.productCoverTextColor("white");
    build.productCoverEffect("Fire");
    build.productCoverTextFont("Kanit Thin");
    build.productCoverTextWeight(100);
    
    build.productUISidebar(true);
    build.productUICompass(true);
    build.productUITextbox(true);            

    build.productUISaveLoad(true);
    //build.productUIUpdate(true);
    //build.productUIClassic(true);
    build.productUIUndoRedo(true);
    
    build.endProduct();
    buf.add(0);  // terminate string
}

bool IFGLK::getProductInfo(ProductInfo& pi)
{
    LOG3("XVAN, ", "getProductInfo");

    GrowString gs;
    _buildProductInfoJSON(gs);

    pi._changed = true;
    pi._json = gs.start();
    return true;
}


// ----------------- GLK handlers -----------------------------


void IFGLK::select(event_t* ev)
{
    // task thread
    if (_shutdown) glk_exit();

    ev->type = 0;

    if (_inbufsz == 2)
    {
        // char
        // XX don't wait for char, pretend newline pressed
        ev->type = evtype_CharInput;
        ev->val1 =  '\n'; 
        
        LOG4("GLK, ", "char in");
    }    
    else if (_inbuf)
    {
        waitForInput();

        LOG4("GLK, ", "have input \"" << _inbuf << "\"");
            
        ev->type = evtype_LineInput;
        ev->val1 = strlen(_inbuf);
    }

    ev->win = (winid_t)_inputWindow;            
}

void glk_tick()
{
}

void glk_stylehint_set(glui32 wintype, glui32 styl, glui32 hint,
                       glsi32 val)
{
    // ignore
}


winid_t glk_window_open(winid_t split, glui32 method, glui32 size,
                        glui32 wintype, glui32 rock)
{
    winid_t w = new glk_window_struct;
    w->_split = split;
    w->_method = method;
    w->_size = size;
    w->_type = wintype;
    w->_rock = rock;

    // XX
    w->setSize(4096);

    // debugging...
    if (wintype & wintype_TextBuffer) w->_name = "textbuf";
    else if (wintype & wintype_TextGrid) w->_name = "textgrid";

    theMS->addWindow(w);

    return w;
}

void glk_window_close(winid_t win, stream_result_t *result)
{
    theMS->removeWindow(win);
}

void glk_set_window(winid_t win)
{
    LOG4("GLK, ", "set window " << win->_name);
    theMS->setWindow(win);
}

void glk_put_char(unsigned char c)
{
    theMS->_putchar((char)c);
}

void glk_put_string(char *s)
{
    theMS->_puts(s);
}

void glk_request_line_event(winid_t win, char *buf, glui32 maxlen,
                            glui32 initlen)
{
    LOG4("GLK, ", "request line");
    theMS->_inbuf = buf;
    theMS->_inbufsz = maxlen;
}

void glk_request_char_event(winid_t win)
{
    LOG2("GLK, ", "request char");
    theMS->_inbufsz = 2;
}

void glk_exit()
{
    LOG2("GLK, ", "exit");
    longjmp(theMS->_jmpExit, 1);
}

void glk_select(event_t* ev)
{
    LOG4("GLK, ", "select");
    theMS->select(ev);
}

void glk_window_clear(winid_t win)
{
    LOG3("GLK, ", "window clear");
}

void glk_set_style(glui32 styl)
{
    LOG4("GLK, ", "set style");
}

void glk_window_move_cursor(winid_t win, glui32 xpos, glui32 ypos)
{
    LOG4("GLK, ", "move cursor");
}

#if 0
void ms_showpic(type32 c,type8 mode)
{
    theMS->_showpic(c, mode);
}

void ms_playmusic(type8 * midi_data, type32 length, type16 tempo)
{
    LOG3("MS ", "music");
}

#endif
