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


#include <stdio.h>
#include <string.h>
#include "cutils.h"

/* Here we have a dummy back-end "game". All it does is echo back what
 * it has received.
 *
 * The code here can compile on it's own, and will make a (simple) console
 * program. However, if IFI_BUILD is defined, it will attach to IFI
 *
 * The first part under IFI_BUILD is glue to attach to IFIClient.
 *
 */

#define IFI_TEST

#ifdef IFI_BUILD

#include "ificlient.h"
#include "ifiglue.h"
#include "ifihandler.h"
#include "logged.h"

struct AClient: public IFIClient
{
    void coopWork() override;

    ~AClient() {}

    Ctx*        _ifictx = 0;
};


// so that we can get it from glue
IFIClient* ifi;
static int moveCount;

// our reference to ifi
#define AIFI  ((AClient*)ifi)

#define TAG  "Dummy, "

struct Handler: public IFIHandler
{
    AClient* _ifi;

    bool ifiCommand(const string&) override { return true; }
    bool ifiMoves(int n) override
    {
        if (n)
        {
            ifi->emitSingleResponse(IFI_MOVES, var(moveCount));
        }
        return true;
    }
    
    bool ifiMeta(bool v) override
    {
        // send meta response
        GrowString js;
        buildJSONStart(js);
        JSONWalker::addStringValue(js, IFI_AUTHOR, "by Nobody");
        JSONWalker::addStringValue(js, IFI_TITLE, "Dummy Game");

        // enable features
        JSONWalker::addBoolValue(js, IFI_UI_TEXTINPUT, true);
        JSONWalker::addBoolValue(js, IFI_UI_COMPASS, true);
        JSONWalker::addBoolValue(js, IFI_UI_RESTART, true);
        JSONWalker::addBoolValue(js, IFI_SAVELOAD, true);

        buildJSONEnd();

        GrowString j1;
        buildJSONStart(j1);
        JSONWalker::addKeyObject(j1, IFI_META, js.start());
        buildJSONEnd();
        _ifi->emitResponse(j1.start());
        return true;
    }
        
};

Handler ifiHandler;

IFI* IFI::create(IFI::Ctx* ctx)
{
    LOG1(TAG, "loading");
    
    ifi = new AClient();

    if (ctx)
    {
        if (ctx->_p)
        {
            // if we want to run in coop mode
            ifi->_coop = true;
            ifi->_pump = ctx->_p;
        }
    }

    AIFI->_ifictx = ctx;
    ifiHandler._ifi = AIFI;
    
    return ifi;
}

int IFIClient::client_main(int argc, char** argv)
{
  // arrive here, then go to the original main, which is renamed to ifi_main
  // to avoid conflict with ::main in the front end.
  return ::main(argc, argv);
}

#endif // IFI_BUILD

/* The remainder of this file can compile standalone and run as a console
 * program.
 */

static void prompt()
{
#ifndef IFI_BUILD    
    // only need to prompt in the standalone case, because
    // when connected to the IFI, the front end may have the input
    // in a totally different window, which may (or may not) prompt
    // the user in its own way.
    putchar('>');
    putchar(' ');
    fflush(stdout);
#endif
}


bool ifiTest(const char* cmd)
{
    bool r = false;
#ifdef IFI_TEST
    // implement various tests initiated by commands
    if ((r = !strcmp(cmd, "#text1")))
    {
        static const char* latin[] = 
            {
                "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nunc nulla nibh, varius eu tellus eget, volutpat maximus mauris. Aliquam vehicula eu mi non tempor. Aenean egestas finibus scelerisque. Maecenas vestibulum sed enim in placerat. Ut pulvinar arcu augue, ut porttitor massa dictum id. Nullam vel erat sodales, luctus ante at, iaculis ante. Vestibulum scelerisque ac nunc in tincidunt. Aenean posuere efficitur euismod. Donec fermentum nunc in tristique congue.",
                
                "Nullam imperdiet dapibus diam, sed convallis orci posuere nec. Cras finibus lacinia gravida. Quisque viverra augue vitae mauris eleifend facilisis. Mauris eu nisl vel purus mattis congue ut vel dolor. Pellentesque nec efficitur velit. Nullam et pellentesque leo, at varius metus. Nullam eget risus ornare justo rutrum consequat. Sed ullamcorper felis iaculis purus vehicula, non efficitur libero congue. Aliquam erat volutpat. Aliquam sed fringilla nisi. Etiam sit amet arcu tempus, finibus justo sed, sagittis augue. Duis est ex, dapibus sed dapibus eu, efficitur tincidunt ante. Donec molestie turpis et maximus sagittis. Vivamus at lorem nec ligula porta lacinia vel ut orci. Nam in ex et nulla suscipit laoreet. Lorem ipsum dolor sit amet, consectetur adipiscing elit.",

                "Donec ultrices, nunc in placerat aliquet, sapien velit gravida tellus, eget lobortis libero felis vel dui. Nulla facilisi. Mauris cursus suscipit lobortis. Integer congue metus at tellus auctor, in sodales enim ullamcorper. Curabitur vitae lorem facilisis, tincidunt mauris a, porttitor metus. Praesent tincidunt quam vitae scelerisque vulputate. In elit leo, finibus ac purus at, euismod finibus est. Vestibulum ligula sapien, rutrum non justo sed, tempus congue lectus. Nulla mollis, nibh eget accumsan ultricies, eros ante sagittis metus, ac suscipit tellus nunc et quam. Praesent dignissim quam congue lectus vulputate, id pretium felis ultrices.",

                "Pellentesque et laoreet dui. Etiam eget consectetur turpis, at viverra dui. In id porta nisl, quis convallis odio. Nam et dignissim metus, porttitor accumsan diam. Duis id dolor sit amet arcu rutrum iaculis. Maecenas ac enim semper, ornare augue a, finibus libero. Maecenas pharetra non dolor in eleifend.",

                "Fusce convallis a ex et dictum. Etiam posuere lorem vel maximus euismod. Proin sollicitudin pharetra vehicula. Nulla ultrices massa vitae scelerisque varius. Nam porta lectus pretium efficitur laoreet. Curabitur nisi enim, varius nec ante nec, molestie aliquam neque. Proin nec sagittis nunc. Cras et gravida lectus, aliquet interdum orci. Nunc ex augue, vestibulum id imperdiet id, ultrices eu dui. Fusce et nunc ut nisl tincidunt euismod. Nunc eleifend imperdiet quam in placerat. Sed in egestas mauris. Proin eu sem pulvinar, elementum turpis in, fermentum augue.",
            };

        for (int i = 0; i < ASIZE(latin); ++i)
            ifi->emitSingleResponse("text", latin[i]);
    }
    else if ((r = !strcmp(cmd, "#text2")))
    {
        static const char* words[] =
            {
                "the ", "[cat](examine cat) ", "sat ", "on ", "the ", "[mat](examine mat)."
            };

        for (int i = 0; i < ASIZE(words); ++i)
            ifi->emitSingleResponse("text", words[i]);
        
    }
    else if ((r = !strcmp(cmd, "#choice1")))
    {
        const char* c1 = "{\"choice\":[{\"text\":\"Go North\"},{\"text\":\"Go South\"}]}";
        LOG3(TAG "emit json ", c1);
        ifi->emitResponse(c1);
    }
    else if ((r = !strcmp(cmd, "#choice2")))
    {
        const char* c1 = "{\"choice\":{\"text\":\"What to do now?\",\"choice\":[{\"text\":\"Decide to go on\",\"chosen\":\"Go North\"},{\"text\":\"Give up and go back\",\"chosen\":\"Go South\"}]}}";
        LOG3(TAG "emit json ", c1);
        ifi->emitResponse(c1);
    }
    else if ((r = !strcmp(cmd, "#choice3")))
    {
        const char* c1 = "{\"choice\":{\"text\":\"Here are your choices:\",\"choice\":[{\"text\":\"A command\",\"chosen\":\"Look at me\"},{\"text\":\"Do some JSON\",\"chosen\":{\"wibble\":1,\"foo\":2}}]}}";        
        LOG3(TAG "emit json ", c1);
        ifi->emitResponse(c1);
    }
    else if ((r = !strcmp(cmd, "#choice4")))
    {
        const char* c1 = "{\"choice\":[{\"text\":\"Press a key to continue\",\"chosen\":\"{}\"}]}";
        LOG3(TAG "emit json ", c1);
        ifi->emitResponse(c1);
    }
    else if ((r = !strcmp(cmd, "#choice5")))
    {
        const char* c1 = "{\"choice\":[{\"text\":\"Go North\"},{\"text\":{\"color\":\"blue\",\"text\":\"Go South\"}}]}";
        LOG3(TAG "emit json ", c1);
        ifi->emitResponse(c1);
    }
    else if ((r = !strcmp(cmd, "#choice6")))
    {
        const char* c1 = "{\"choice\":{\"text\":{\"italic\":\"true\",\"color\":\"green\",\"text\":\"What now?\"},\"choice\":[{\"text\":\"Decide to go on\",\"chosen\":\"Go North\"},{\"text\":\"Give up and go back\",\"chosen\":\"Go South\"}]}}";
        LOG3(TAG "emit json ", c1);
        ifi->emitResponse(c1);
    }
    
#endif
    return r;
}

void AClient::coopWork()
{
    LOG1(TAG "AClient::coopwork", "");

    for (;;)
    {
        const char* r = ifi->getRequest();
        if (!r) break;
        
        LOG1(TAG "interative, request: ", r);
        //kli->_ifiHandler.handle(r);
    }
};

int main(int argc, char** argv)
{
    // OK, not much of a game :-)

#ifdef IFI_BUILD
    // handle any initial JSON
    for (int i = 1; i < argc; ++i)
    {
        if (!strcmp(argv[i], "-e") && i < argc-1)
        {
            char* js = argv[++i];
            LOG1(TAG, "initial json " << js);
            ifiHandler.handle(js);
        }
    }
#endif

    ifiHandler._startDone = true;

    for (;;)
    {
#ifdef IFI_BUILD

        //int foo; LOG1(TAG "Stack at ", &foo);

        // here is one difference. If we want to handle JSON requests
        // as well as regular "commands", there has to be a handler somewhere.
        const char* ins;
        for (;;)
        {
            ins = ifi->getRequest();
            if (ins) break;
            LOG1(TAG "Pumping", "");
            if (ifi->_pump) (ifi->_pump)();
        }

        ifiHandler.handle(ins);

#endif

        prompt();

        ++moveCount;

        char buf[1024];
        char* s = gets_s(buf, sizeof(buf));
        if (!s || !strcmp(s, "q")) break;
        if (!*s) continue;

        if (ifiTest(s)) continue;
        printf("You said, \"%s\".\n", s);

    }

    return 0;
}



