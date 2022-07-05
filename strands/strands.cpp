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


#ifdef STANDALONE
#include <time.h>
#endif

#include "fdz.h"
#include "strands.h"
#include "logged.h"
#include "kl.h"

#define DEFAULT_STORY_FILE   "story.str"
#define DEFAULT_STORY_BINARY "story.stz"

#define DEFAULT_VOICE_FILE   "voice.txt"

namespace ST
{
Term::Terms Term::_allTerms(&Term::compareLess);
};

#ifdef STANDALONE


#include "strandi.h"
#include "pstrands.h"

bool writeBinary(GrowString& buf, const char* fname, bool compress)
{
    FD out;
    bool r = out.open(fname, FD::fd_new);
    if (r)
    {
        if (compress)
        {
            r = writez(out, (unsigned char*)buf.start(), buf.size());
        }
        else
        {
            r = out.write((unsigned char*)buf.start(), buf.size());
        }
    }
    return r;
}

bool writeVoices(ST::ParseStrands::VoiceSet& vs, const char* fname)
{
    FD out;
    bool r = out.open(fname, FD::fd_new);
    if (r)
    {
        FDBuf fout(out);
        for (auto& v : vs)
        {
            fout.printf("%s: rec:%s\n", v._actor.c_str(), v._filename.c_str());
            fout.write((const unsigned char*)v._speech.c_str(), v._speech.size());
            fout.putc('\n');
            fout.putc('\n');
        }
    }
    return r;
}

struct KLEnv
{
    typedef std::string string;

    struct CapStdStream: public StdStream
    {
        string _buf;
        bool _emit(StdStreamBuf* buf) override
        {
            _buf += (const char*)*buf;
            return true;
        }
    };

    
    KL                  _kl;
    ST::Strandi*        _strandi;

    KLEnv(ST::Strandi& s) : _strandi(&s)
    {
        extern void initKLStrandi(KL* host, ST::Strandi* s);
        initKLStrandi(&_kl, _strandi);
    }

    bool loadFile(const string& f, int errlev = 1)
    { return _kl.loadFile(f, _kl._env, errlev); }

    ST::Capture* evalKL(const string& buf, ST::Capture* args)
    {
        //LOG1(TAG "eval KL ", buf);
            
        int cc = 0;
            
        ST::Capture* r = new ST::Capture;

        if (args)
        {

            List arglist;
            List::iterator it(arglist);
            LValue lv(it);
            for (auto& e : args->_elts)
            {
                if (e._v)
                {
                    lv = *KL::varToTerm(e._v);
                    ++cc;
                }
                else if (e._term)
                {
                    ParseContext ctx(&_kl);
                    Term t = Symbol(e._term->_name.c_str()).find(&ctx);
                    if (t)
                    {
                        lv = *t;
                        ++cc;
                    }
                }
                else
                {
                    // append string if valid
                    if (e._s.size())
                    {
                        lv = Stringt(e._s);
                        ++cc;
                    }
                }
            }

            if (cc)
            {
                ParseContext ctx(&_kl);
                Term s = Symbol("IT").intern(&ctx);

                //Tree e;
                //List et(e, _kl->_env._env);
                //Env env(et, _kl->_env._args);
                _kl._bindvar(s, &arglist, _kl._env, true, false);
            }
        }

        CapStdStream tout;
        StdStream* old = _kl.setOutput(&tout);
        _kl.loadString(buf.c_str(), _kl._env); // eval!
        _kl.setOutput(old);
        r->add(tout._buf); // ignore if empty

        //LOG1(TAG "eval KL result ", tout._buf);

#if 0            
        if (v)
        {
            LOG1(TAG "evalKL result ", v->toString());
                     
            // return values are ignored unless a string
            if (v.isString())
            {
                // avoid quotes in standard KL stringification
                r->add(v.asString().c_str());
            }
        }
#endif
        
        if (r->empty())
        {
            delete r;
            r = 0;
        }
        
        return r;
    }
};

int main(int argc, char** argv)
{
    int debug = 0;
    Logged initLog;
    ST::ParseStrands ps;

    // write all files out as compressed story
    bool emitBin = false;
    bool loadGameFiles = true;
    bool compress = true;

    const char* initFilename = "init";

    std::vector<std::string> files;
    
    for (int i = 1; i < argc; ++i)
    {
        if (argv[i][0] == '-')
        {
            if (!u_stricmp(argv[i], "-d"))
            {
                debug = 1;
                if (i < argc-1 && u_isdigit(*argv[i+1]))
                {
                    debug = atoi(argv[++i]);
                }
            }
            else if (!u_stricmp(argv[i], "-bin"))
            {
                // collect up all the source code
                // as we read it in.
                ps._collectSource = true;
                emitBin = true;
            }
            else if (!u_stricmp(argv[i], "-only"))
            {
                // do not load other game files by processing
                // TERM_GAME_FILES
                // this option is useful when all files are merged into one
                loadGameFiles = false;
            }
            else if (!u_stricmp(argv[i], "-nocompress"))
            {
                // if emitting bin, do not compress
                compress = false;
            }
            else if (!u_stricmp(argv[i], "-genvoice")) ps._collectVoices = true;
            else if (!u_stricmp(argv[i], "-dump")) ps._dump = true;
            else
            {
                printf("unrecognised option '%s'\n", argv[i]);
            }
        }
        else
        {
            files.push_back(argv[i]);
        }
    }

    if (!files.size())
    {
        // look for default story file. 
        if (FD::existsFile(DEFAULT_STORY_FILE))
            files.push_back(DEFAULT_STORY_FILE);
    }
    
    if (!files.size())
    {
        printf("Usage: %s [-bin [-nocompress]] [-genvoice] [-dump] [-only] [-d [n]] file...\n", argv[0]);
        return -1;
    }

    if (debug)
    {
        printf("debug level %d\n", debug);
        ps._debug = debug;
        Logged::_logLevel = debug;
    }

    bool v = ps.loadFiles(files, 0, loadGameFiles);

    if (v)
    {
        bool v = ps.validate();

        if (v && ps._startTerm)
        {
            if (emitBin)
            {
                // emit compressed binary
                v = writeBinary(ps._sourceCollector,
                                DEFAULT_STORY_BINARY,
                                compress);
                if (!v)
                    printf("error writing binary '%s'\n", DEFAULT_STORY_BINARY);
            }
            else if (ps._collectVoices)
            {
                writeVoices(ps._voiceSet, DEFAULT_VOICE_FILE);
            }
            else
            {
                ST::Strandi si(&ST::Term::_allTerms);
                si.setdebug(debug);

                KLEnv kle(si);
                kle.loadFile(initFilename, 2);  // soft error

                using namespace std::placeholders;
                si.setEvaluator(std::bind(&KLEnv::evalKL, &kle, _1, _2));

                time_t tt;
                si.randomise(time(&tt));

                if (!si.start(ps._startTerm))
                {
                    v = false;
                    ERR0("cannot start");
                }
            }
        }
    }

    return v ? 0 : 1;
}


#endif // STANDALONE
