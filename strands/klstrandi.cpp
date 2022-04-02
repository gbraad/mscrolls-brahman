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


#include "kllib.h"

#ifdef IFI_BUILD
#include "ificlient.h"
#include "ifihandler.h"

extern IFIClient* ifi;
extern IFIHandler* ifih;
#endif // IFI_BUILD

#include "strandi.h"

namespace KLS
{
#include "tformat.h"
};


struct KLStrandi: public KLLib
{
    typedef KLStrandi Self;
    typedef Term (Self::*PrimFn)(List::iterator&, Env&);
    typedef PCallFn<PrimFn, Self> PC;
    typedef ST::Strandi Strandi;

    Strandi*        _strandi;
    
    KLStrandi(KL* host, Strandi* s) : KLLib(host), _strandi(s) { _init(); }
    const char* libname() const override { return "klstrandi"; }
    void destroy(Blob* b) override { delete b; }
    string asString(const Blob* b) const override { return "blob"; }

    string _textify(const Term& t)
    {
        string s;
        if (t)
        {
            if (t.isList())
            {
                for (List::iterator it(t.asList()); it; ++it)
                {
                    string si = _textify(*it);
                    if (si.size())
                    {
                        if (!s.empty()) s += ' ';
                        s += si;
                    }
                }
            }
            else if (t.isSymbol())
            {
                // copes with null
                s = _strandi->textify(ST::Term::find(t.asSymbol().label()));
            }
            else
            {
                s = t.toStringRaw();
            }
        }
        return s;
    }

    Term _primTextifyFn(List::iterator& ai, Env& env)
    {
        // (textify term)
        Term r;
        if (ai)
        {
            Term t = EVALAI;
            string s = _textify(t);
            if (!s.empty()) r = Stringt(s);
        }
        return r;
    }

    Term _primTermIDFn(List::iterator& ai, Env& env)
    {
        // (termid TERMNAME)  -> number
        Term r;
        if (ai)
        {
            Term t = EVALAI;
            if (t.isSymbol())
            {
                ST::Term* st = ST::Term::find(t.asSymbol().label());
                if (st)
                {
                    r = Int(st->_uiID);
                }
            }
        }
        return r;
    }

    Term _primCapsWordsFn(List::iterator& ai, Env& env)
    {
        // (capswords t)
        Term r;
        if (ai)
        {
            Term t = EVALAI;
            string s = CapitaliseStartWords(t.toStringRaw());
            r = Stringt(s);
        }
        return r;
        
    }

    Term _primPlainToHTMLFn(List::iterator& ai, Env& env)
    {
        Term r;
        
        return r;
    }
    
    Term _primHTMLToPlainFn(List::iterator& ai, Env& env)
    {
        Term t = EVALAI;
        KLS::TextFormat tf;
        tf.setHTML(t.toStringRaw());
        Term r = Stringt(tf._text);
        //LOG1("HTMLTOPLain = ", tf._text);
        return r;
    }

    Term _primRunTermFn(List::iterator& ai, Env& env)
    {
        // (runtime name)
        Term t = EVALAI;
        string tname = t.toStringRaw();
        string s;
        if (!tname.empty())
        {
            s = _strandi->runSingleTermCap(tname);
            //LOG3("KLStrandi, eval sym ", tname << " = " << s);
        }
        Term r = Stringt(s);        
        return r;
    }

    Term _primUndoTermFn(List::iterator& ai, Env& env)
    {
        Term t = EVALAI;
        string tname = t.toStringRaw();

        int r = 0;
        if (!tname.empty())
        {
            if (_strandi->undoToTerm(tname)) r = 1;
        }
        return Int(r);
    }

    void _init()
    {
        Tree g;

        // data
        DEF_PRIM(Textify, "textify");
        DEF_PRIM(TermID, "termid");
        DEF_PRIM(CapsWords, "capswords");
        DEF_PRIM(HTMLToPlain, "htmltoplain");
        DEF_PRIM(PlainToHTML, "plaintohtml");
        DEF_PRIM(RunTerm, "runterm");
        DEF_PRIM(UndoTerm, "undoterm");

        _host->_env._env = List(g, *_host->_env._env);

    }

};

void initKLStrandi(KL* host, ST::Strandi* s)
{
    LOG3("KLStrandi", "init");
    static KLStrandi klstrandi(host, s);
}


