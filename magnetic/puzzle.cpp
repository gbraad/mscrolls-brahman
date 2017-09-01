/**
 *
 *    _    __        _      __                           
 *   | |  / /____   (_)____/ /_      __ ____ _ _____ ___ 
 *   | | / // __ \ / // __  /| | /| / // __ `// ___// _ \
 *   | |/ // /_/ // // /_/ / | |/ |/ // /_/ // /   /  __/
 *   |___/ \____//_/ \__,_/  |__/|__/ \__,_//_/    \___/ 
 *                                                       
 *  Copyright (©) Voidware 2016-2017.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 * 
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 * 
 *  THE SOFTWARE IS PROVIDED "AS IS," WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 * 
 *  contact@voidware.com
 */

#include "puzzle.h"
#include "logged.h"
#include "matcol.h"

#ifndef STANDALONE
#include "ifproduct.h"
#endif

struct BoulderPuzzle: public Puzzle
{
    /*
      1: intro
      2: hoe
      4: rake
      8: suggest tied
    */

    BoulderPuzzle() : Puzzle("boulders") {}

    bool precondition() override
    {
        return has(_name).isExplored() && !way(28, 31); // foothills->track
    }

    void action(int act) override
    {
        // perform activities as a result of the puzzle state
        switch (act)
        {
        case 2: _action("lever boulders with hoe.", act); break;
        case 4: _action("lever boulders with rake.", act); break;
        case 16:
            {
                actIsoGet(has("black shirt"));
                _action("tie hoe and rake together with the shirt.");
                if (connected(has("hoe"), has("rake"))) state(act);
            }
            break;
        case 32:
            _action("lever boulders with rake.", act);
            break;
        }
    }

    void run() override
    {
        // run the puzzle

        if (state(0, ~1))
        {
            text("Looks like you need to lever the [boulder](do boulders) out of the way with something.", 1);
        }
        if (state(1, 2) && has("hoe"))
        {
            text("Try to [lever](do boulders 2) the boulder with the hoe.");
        }
        if (state(1, 4) && has("rake"))
        {
            text("Try to [lever](do boulders 4) the boulder with the rake.");
        }
        if (state(7, 8))
        {
            text("Maybe if the hoe and rake were tied together it might be strong enough to lever the [boulder](do boulders).", 8);
            return;
        }
        if (state(8, 0) && notConnected(has("hoe"), has("rake")) && has("black shirt"))
        {
            text("How about [tying](do boulders 16) the hoe and rake together with the shirt");
        }
        if (state(8, 0) && connected(has("hoe"), has("rake")))
        {
            text("Now try [levering](do boulders 32) the boulder with both the hoe and rake together.");
            return;
        }
    }
};

struct GuruPuzzle: public Puzzle
{
    GuruPuzzle() : Puzzle("guru") {}

    bool precondition() override
    {
        // wearing the wristband with nothing tied to it.
        IItem w("wristband");
        return Puzzle::precondition() && w.isWorn() && !connections(w);
    }

    void action(int act) override
    {
        switch (act)
        {
        case 4: // hide wristband
            {
                IItem s = actIsoGet(has("black shirt"));
                _action("cover wristband with black shirt");
                if (connected(s, "wristband")) state(act);
            }
            break;
        }
    }

    void run() override
    {
        if (state(0, 1))
        {
            text("the guru is laughing at the wristband you're wearing.", 1);
            return;
        }
        if (state(1, 2))
        {
            text("Perhaps, if you could hide the wristband somehow.", 2);
            return;
        }
        if (state(2, 4) && has("black shirt"))
        {
            text("try [covering](do guru 4) the wristband with the shirt");
        }
    }
};

struct RocksPuzzle: public Puzzle
{
    RocksPuzzle() : Puzzle("rocks") {}

    void run() override
    {
        if (state(0, 1)) text("maybe [climb](climb rocks) rocks.", 1);
    }
};

struct SnowPuzzle: public Puzzle
{
    SnowPuzzle() : Puzzle("snow") {}

    bool precondition() override
    {
        // does not have to be explored
        return has(_name);
    }

    bool match(const char* name) const override
    {
        // can click on snow or plateau
        return equalsIgnoreCase(_name, name)
            || equalsIgnoreCase(name, "plateau");
    }

    void action(int act) override
    {
        switch (act)
        {
        case 1:
            actIsoGet(has("black shirt"));
            _action("wear black shirt", act); 
            break;
        case 2:
            actIsoGet(has("jeans"));
            _action("wear jeans", act); 
            break;
        }
    }

    void run() override
    {
        if (state(0, 0)) 
        {
            IItem s = has("black shirt");
            if (s && !s.isWorn()) text("maybe [wear](do snow 1) the shirt");

            IItem j = has("jeans");
            if (j && !j.isWorn()) text("maybe [wear](do snow 2) the jeans");
            
        }
    }
};

struct KronosPuzzle: public Puzzle
{
    KronosPuzzle() : Puzzle("magician") {}

    void action(int act) override
    {
        switch (act)
        {
        case 1:
            actIsoGet(has("aerosoul"));
            _action("press nozzle", act);
        }
    }

    void run() override
    {
        if (has("magician").isDead())
        {
            if (has("aerosoul"))
            {
                if (state(0, 1))
                {
                    text("maybe press [aerosoul nozzle](do magician 1)");
                }
            }
            else
            {
                text("You need the aerosoul!\n");
            }
        }
    }
};


struct AdventurerPuzzle: public Puzzle
{
    AdventurerPuzzle() : Puzzle("adventurer") {}

    bool precondition() override
    {
        IItem a = has(_name);
        return a.isExplored() && !a.isDead();
    }

    void run() override
    {
        if (state(0, 1)) text("maybe [say](adventurer, hello) hello", 1);
        if (state(0, 2))
        {
            if (has("chest").carried())
                text("maybe [give](give chest to adventurer) the chest to the adventurer");
        }
    }
};

struct HorsePuzzle: public Puzzle
{
    HorsePuzzle() : Puzzle("horse") {}

    bool precondition() override
    {
        IItem a = has(_name);
        return a.isExplored() && !a.isDead() && IItem("adventurer").isDead();
    }

    void run() override
    {
        if (IItem("princess").carried())
            text("maybe [put](put princess on horse) princess on horse.");
        
        if (state(0, 1)) text("maybe [say](horse, hello) hello", 1);
        if (state(0, 2))
        {
            LOG3("player on ", get_object_player_on());
            IItem ploc(find_item(get_object_player_on()));
            if (ploc)
            {
                if (ploc == has("horse"))
                {
                    text("maybe [get off](get off horse) horse");
                }
            }
            else
            {
                text("maybe [ride](ride horse) the horse");
            }
        }
    }
};

struct WallPuzzle: public Puzzle
{
    WallPuzzle() : Puzzle("low wall") {}

    void run() override
    {
        if (state(0, 1)) text("you could probably [break](break wall) the wall");
    }
};

struct PedestalPuzzle: public Puzzle
{
    PedestalPuzzle() : Puzzle("pedestal") {}

    void run() override
    {
        if (state(0, 1)) text("maybe you can [move](push pedestal) the pedestal", 1);
    }
};

struct SlopePuzzle: public Puzzle
{
    SlopePuzzle() : Puzzle("slope") {}

    void action(int act) override
    {
        switch (act)
        {
        case 1:
            actWear("boots");
            _action("climb slope", act);
            break;
        }
    }

    void run() override
    {
        if (state(0, 1)) text("maybe try to [climb](do slope 1) the slope");
    }
};

struct PrincessPuzzle: public Puzzle
{
    PrincessPuzzle() : Puzzle("princess") {}

    void action(int act) override
    {
        switch (act)
        {
        case 1:
            actIsoGet(has("black shirt"));
            _action("tie shirt to bed", act);
            break;
        }
    }

    void run() override
    {
        IItem p("princess");
        if (!p.carried()) text("maybe [get princess](get princess), ready for a daring rescue.");
        else
        {
            IItem bed, shirt;
            if ((bed = has("poster bed")) && ((shirt = has("shirt"))))
            {
                text("maybe [tie](do princess 1) shirt to bed", 1);
            }
            else if (get_current_room() == 41 && has("shirt"))
            {
                text("maybe [untie](untie shirt then wear it) the shirt.");
            }
        }
    }
};

struct BoardsPuzzle: public Puzzle
{
    BoardsPuzzle() : Puzzle("varnished boards") {}

    void run() override
    {
        if (state(0,1)) text("maybe you can [move](move boards) the boards.", 1);
        if (state(1,2))
        {
            IItem wd("wooden door");
            if (wd.isOpen())
            {
                text("maybe the door is in the way, try [closing](close door) the door.");
            }
        }
    }
};

struct LiftButtonPuzzle: public Puzzle
{
    LiftButtonPuzzle() : Puzzle("lift door") {}

    void run() override
    {
        text("maybe [slide](slide lift door) the lift door");
    }
};


struct LiftDoorPuzzle: public Puzzle
{
    LiftDoorPuzzle() : Puzzle("lift button") {}

    void run() override
    {
        if (state(0,1)) text("maybe [press](press lift button) the lift button.", 1);
    }
};

struct LumpsPuzzle: public Puzzle
{
    LumpsPuzzle() : Puzzle("lumps") {}

    void run() override
    {
        if (get_current_room() == 87)
        {
            IItem l(has("lumps"));
            if (l && !l.carried())
            {
                if (has("trowel"))
                {
                    text("maybe [dig](get lumps with trowel) them out with the trowel");
                }
                else
                {
                    text("you're going to need something like a trowel to dig them out.");
                }
            }
        }
    }
};

struct PaperWallPuzzle: public Puzzle
{
    PaperWallPuzzle() : Puzzle("paper wall") {}

    void run() override
    {
        text("maybe [lean](lean on paper wall) on the wall.");

        if (has("paper wall"))
        {
            if (has("trowel"))
            {
                text("maybe [tear paper wall](tear paper wall with trowel) with the trowel.");
            }
            else
            {
                text("you need something to tear the paper wall");
            }
        }
    }
};

struct SpellPuzzle: public Puzzle
{
    SpellPuzzle() : Puzzle("spell") {}

    void run() override
    {
        if (has("tomes"))
            text("why not cast [spell](cast spell on tomes) on tomes");
    }
};

struct TomesPuzzle: public Puzzle
{
    TomesPuzzle() : Puzzle("tomes") {}

    void run() override
    {
        if (!has("tomes").isClosed())
            text("why not [read](read tomes) the tomes");
    }
};


struct KnockerPuzzle: public Puzzle
{
    KnockerPuzzle() : Puzzle("knocker") {}

    void run() override
    {
        text("maybe [knock](knock on doors) on doors");
    }
};

struct RopePuzzle: public Puzzle
{
    RopePuzzle() : Puzzle("hook") {}

    void action(int act) override
    {
        switch (act)
        {
        case 1:
            actGet(IItem("rope"));
            _action("climb down then drop rope");
            break;
        }
    }

    void run() override
    {
        IItem rope("rope"); // can be non-local
        IItem hook(has("hook"));
        IItem pw(has("paper wall"));
        
        if (connected(rope, hook))
        {
            if (!pw)
            {
                text("maybe [climb down](do hook 1)");
            }
        }
        else if (rope)
        {
            text("maybe [tie](tie rope to hook) the rope to the [hook](do hook)", 1);
        }
        else text("You need a rope or something to tie to this [hook](do hook)");
    }
};

static uchar* atDragon2()
{
    if (prog_format)
    {
        static int addr;
        if (!addr) addr = get_sym_value("F_DRAG2");
        assert(addr);
        return getcode() + addr;
    }
    else
    {
        return getcode() + 0x1a57;
    }
}

static uchar* atKmeet3()
{
    if (prog_format)
    {
        static int addr;
        if (!addr) addr = get_sym_value("F_KMEET3");
        assert(addr);
        return getcode() + addr;
    }
    else
    {
        return getcode() + 0x1a52;
    }
}

struct DragonPuzzle: public Puzzle
{
    DragonPuzzle() : Puzzle("dragon") {}

    void run() override
    {
        IItem white("white");
        if (white.carried(true)) // x-ray
        {
            if (has("pouch").isClosed())
            {
                text("maybe [open pouch](open pouch)");

                uchar* flagDragon2 = atDragon2();
                if (*flagDragon2 && *flagDragon2 != 0xff) 
                    --*flagDragon2; // give them an extra move to open pouch
                return;
            }
            
            if (state(0, 1))
            {
                text("maybe [illuminate](shine white at shadows) the shadows with the [white]", 1);
                return;
            }
            else if (state(1, 2))
            {
                text("maybe [point out](point at shapes) the hobbits hiding in the shadows", 2);
            }
        }
    }
};

struct CreamDoorPuzzle: public Puzzle
{
    CreamDoorPuzzle() : Puzzle("cream door") {}

    void action(int act) override
    {
        switch (act)
        {
        case 1:
            _action("knock on cream door", act);
            break;
        }
    }
    
    void run() override
    {
        if (has("cream door").isClosedOrLocked())
        {
            if (state(0,1))
            {
                text("maybe [knock](do cream door 1) on the door");
            }
            if (state(1, 2))
            {
                text("maybe [say no](say no)");
            }
        }
    }
};

struct PotPuzzle: public Puzzle
{
    PotPuzzle() : Puzzle("pot") {}

    void action(int act) override
    {
        switch (act)
        {
        case 1:
            actIsoGet(has("trowel"));
            _action("plant the pot plant in the plant pot with the trowel");
            break;
        }
    }
    
    void run() override
    {
        if (has("trowel"))
        {
            text("maybe [plant](do pot 1) the [pot plant] in the [plant pot] with the [trowel]");
        }
    }
};

struct SafePuzzle: public Puzzle
{
    SafePuzzle() : Puzzle("safe") {}

    void run() override
    {
        if (has("safe").isOpen())
        {
            text("maybe [search](search safe) the safe");
        }
    }
};



// PuzzleManager functions that are ifmagnetic independent
// see also puzman.cpp

void PuzzleManager::start(IFMagnetic* host)
{
    _host = host;

    switch (get_game())
    {
    case 1: // the pawn
        add(new BoulderPuzzle);
        add(new GuruPuzzle);
        add(new RocksPuzzle);
        add(new SnowPuzzle);
        add(new KronosPuzzle);
        add(new AdventurerPuzzle);
        add(new HorsePuzzle);
        add(new WallPuzzle);
        add(new PedestalPuzzle);
        add(new SlopePuzzle);
        add(new PrincessPuzzle);
        add(new BoardsPuzzle);
        add(new LiftButtonPuzzle);
        add(new LiftDoorPuzzle);
        add(new LumpsPuzzle);
        add(new PaperWallPuzzle);
        add(new SpellPuzzle);
        add(new TomesPuzzle);
        add(new KnockerPuzzle);
        add(new RopePuzzle);
        add(new DragonPuzzle);
        add(new CreamDoorPuzzle);
        add(new PotPuzzle);
        add(new SafePuzzle);
        break;
    }

    applyGameFixes();
}

#define READW(_a) ((((unsigned int)_a[0])<<8) + _a[1])
#define WRITEW(_a, _v)          \
{                               \
    _a[0] = (_v) >> 8;          \
    _a[1] = (_v);               \
}

const char* PuzzleManager::messageHook(int m)
{
    // NB: game thread
    
    //LOG3("msg# ", m);
    switch (get_game())
    {
    case 1: // the pawn
        if (!prog_format) break;
        if (m == 1105) 
        {
            // revealing a path to the nw
            IItem::getRoom(28).setExit(IItem::dir_nw, 31);
        }
        if (m == 726)
        {
            //LOG3("puz ", "died!, trigger undo");
            _triggerUndo = true;
        }
        if (m == 92)
        {
            // halfway down the tower.
            // provide a click link to continue the puzzle
            return has("princess") ? "%s, rescuing the princess" : 0;
        }
        if (m == 718)
        {
            // vanishes

            // blue key doesn't vanish anymore
            IItem bk("blue key");
            if (bk.isOut())
            {
                bk.resetOut();
                return "worked.";
            }
        }
        if (m == 273)
        {
            // tree trunk room
            if (IItem::getRoom(78).getExit(IItem::dir_d))
            {
                return "%s, or go [down](d)";
            }
        }
        if (m == 414)
        {
            // too dark to see
            if (has("red") && has("green") && has("blue"))
            {
                return "%s. Maybe [mix](mix colours) the colours";
            }
        }
        if (m == 82)
        {
            // narrow track room
            IItem::getRoom(31).setExit(IItem::dir_n, 32);
            
        }
        if (m == 1122)
        {
            // bright light
            if (has("pouch"))
            {
                return "%s. maybe put [white back](put white in pouch then close pouch) in its pouch";
            }
        }
        if (m == 995)
        {
            // magician zaps you back to path

            // reset kronos meet so you get a move before it happens again
            uchar* flagKMeet3 = atKmeet3();
            WRITEW(flagKMeet3, 0);
        }
        if (m == 1161)
        {
            // game end message
        }
    }
    return 0;
}

std::string PuzzleManager::evalUseXwithYSpecial(IItem xi, IItem yi)
{
    string cmd;

    switch (get_game())
    {
    case 1:
        {
            if (xi.wordIs("red") && yi.wordIs("snowman"))
            {
                cmd = "melt snowman with red then get red";
            }
            else if (yi.wordIs("magician"))
            {
                // if they select the potion instead of the bottle, still do it.
                if (xi.wordIs("potion") || xi == IItem("potion bottle"))
                {
                    cmd = "throw potion bottle at magician";
                }
            }
            else if (yi.wordIs("horse"))
            {
                // put X on Horse
                cmd = "put " + xi.toString() + " on " + yi.toString(); 
            }
        }
        break;
    }
    
    return cmd;
}

void PuzzleManager::applyGameFixes()
{
    int g = get_game();
    if (!g) return;
    
    LOG3("Puzzman, ", "applying game fixes");
    switch (g)
    {
    case 1:
        {
            IItem("wooden workbench").setMoveable(); // so auto look under works

            // you could get the fridge!!
            IItem("fridge").setWeight(15);

            // exits from the void
            IItem::getRoom(4).setExit(IItem::dir_n, 3);
            IItem::getRoom(4).setExit(IItem::dir_ne, 16);
            IItem::getRoom(4).setExit(IItem::dir_nw, 6);
        }
        break;
    }
}

std::string PuzzleManager::itemToStringSpecial(IItem ii)
{
    // this is used when we wish to display the string name of an 
    // object or character on the UI.
    //
    // ii.toString() would normally be fine, except for some cases
    // we need to change the string, depending on the game and, 
    // in theory, the situation.

    string s;
    int g = get_game();

    switch (g)
    {
        case 1:
            if (ii.wordIs("magician")) // kronos
            {
                // the root noun for Kronos is "magician"
                // his adjective is "evil"
                // and Kronos is an alias for magician.

                // but we'd like to address him as "Kronos the Magician"
                // for the UI.
                s = "Kronos the Magician";
            }
            else if (ii.wordIs("red")) s = "The Red";
            else if (ii.wordIs("green")) s = "The Green";
            else if (ii.wordIs("blue")) s = "The Blue";
            else if (ii.wordIs("teapot")) s = "A Teapot";
            break;
    }

    // default
    if (s.empty()) s = ii.toString();
    return s;
}

void PuzzleManager::moveUpdate()
{
    switch(get_game())
    {
    case 1:
        {
            if (prog_format) break;
            
            uchar* flagDragon2 = atDragon2();
            uchar* flagDragon3 = getcode() + 0x1a58;

            if (!*flagDragon3)
            {
                // keep resetting 2 to give one more move
                if (*flagDragon2 == 0xff)
                {
                    *flagDragon2 = 7;
                    LOG3("puzzman, dragon2 reset ", *flagDragon2);
                }
            }
            else
            {
                if (*flagDragon3 == 0xff)
                {
                    *flagDragon3 = 0;
                    LOG3("puzzman, dragon3 reset ", *flagDragon3);
                }
            }
        }

        // outmode is the normal/verbose/brief setting
        // controlled by issuing game commands rather than patch memory
        // but here is the address for the pawn anyhow
    
        //uchar* outmode = getcode() + 0x1942;
        //LOG3("outmode, ", (int)*outmode);
        break;
    }
}

#ifndef STANDALONE
static void builderEmitter(char c, void* ctx)
{
    GrowString* buf = (GrowString*)ctx;
    buf->add(c);
}


static void buildCommonProductInfo(ProductInfoBuilder& build)
{
            // UI
    build.productUISidebar(true);
    build.productUICompass(true);
    build.productUITextbox(true);            

    build.productUISaveLoad(true);
    build.productUIUpdate(true);
    build.productUIClassic(true);
    build.productUIUndoRedo(true);
}


void PuzzleManager::buildProductInfoJSON(GrowString& buf,
                                         const string& originalCredits)
{
    // use builder helper, give it our emitter
    ProductInfoBuilder build(builderEmitter, &buf);

    build.beginProduct();

    switch (get_game())
    {
    case 1:
        {
            build.productTitle("The Pawn");
            build.productAuthor("by Magnetic Scrolls");

            GrowString gs;
            const char* c = 
                "<h1>The Pawn<br/><em>by Magnetic Scrolls</em></h1>"
                "<h3>Remastered for 2017</h3>"
        
                "<h4>Strand Games Team</h4>"
                "<ul>"
                "<li>Hugh Steers</li>"
                "<li>Stefan Meier</li>"
                "<li>Rob Steggles</li>"
                "</ul>"
        
                "<h4>Emulator Team</h4>"
                "<ul>"
                "<li>Niclas Karlsson</li>"
                "<li>David Kinder</li>"
                "<li>Stefan Meier</li>"
                "<li>Paul David Doherty</li>"
                "</ul>"

                "<h4>Magnetic Scrolls Team</h4>"
                "<ul>"
                "<li>Anita Sinclair</li>"
                "<li>Ken Gordon</li>"
                "<li>Hugh Steers</li>"
                "<li>Rob Steggles</li>"
                "</ul>"
        
                "<h4>Writing</h4>"
                "<ul>"
                "<li>Rob Steggles</li>"
                "</ul>"
        
                "<h4>Art</h4>"
                "<ul>"
                "<li>Geoff Quilley</li>"
                "</ul>"

                "<h4>Music</h4>"
                "<ul>"
                "<li>John Molloy (remastered music, thanks John!)</li>"
                "</ul>"
        
                "<h4>Testing</h4>"
                "<ul>"
                "<li>Ruben Gordon</li>"
                "<li>Servan Keondjian</li>"
                "<li>Anita Sinclair</li>"
                "</ul>"

                "<h4>Special Thanks To</h4>"
                "<li><p>"
                "Contributors to the <a href=\\\"https://strandgames.com/community\\\">Strand forum</a> and others who've helped with feedback, suggestions and testing."
                "</li></p>"                

                "<h4>Additional Credit</h4>"
                "<ul>"
                "<li>" "<p>"
                "Retro PC fonts from <a href=\\\"http://int10h.org/oldschool-pc-fonts\\\">int10.org</a>."
                "</p>" "</li>"
                "</ul>"
        
                "<h3>Original Platforms (1986)</h3>"
                "<p>"
                "Amiga, Amstrad CPC, Amstrad PCW, Apple2, Archimedes, Atari ST, Atari XL/XE, Commodore 128/ 64, IBM PC, Macintosh, Sinclair QL, Spectrum 128K, Spectrum +3"
                "</p>";
        

            gs.append(c);
            if (!originalCredits.empty())
            {
                gs.append("<h3>Original Game Credits</h3>");

                const char* p = originalCredits.c_str();
                gs.append("<p>\\\"");
                while (*p)
                {
                    // remove any newlines (json doesn't like them)
                    char c  = *p++;
                    if (c == '\n') c = ' ';
                    gs.add(c);
                }
                gs.append("\\\"</p>");
            }
            gs.add(0);
            build.productCredits(gs.start());

            const char* colname = "deep orange";
            build.productThemePrimeColor(MatCol::get(colname).toString());
            build.productThemeContrastColor(MatCol::get(MatCol::getContrastName(colname)).toString());

            //build.productThemeTabColor("white");

            // cover page
            build.productCoverTextColor("white");
            build.productCoverEffect("Fire");
            build.productCoverTextFont("Kanit Thin");
            build.productCoverTextWeight(100);

            // add features common to magnetic games
            buildCommonProductInfo(build);

        }
        break;
    case 2:
        {
            build.productTitle("The Guild of Thieves");
            build.productAuthor("by Magnetic Scrolls");

            GrowString gs;
            const char* c = 
                "<h1>The Guild of Thieves<br/><em>by Magnetic Scrolls</em></h1>"
                "<h3>Remastered for 2017</h3>"
        
                "<h4>Strand Games Team</h4>"
                "<ul>"
                "<li>Hugh Steers</li>"
                "<li>Stefan Meier</li>"
                "<li>Rob Steggles</li>"
                "</ul>"
        
                "<h4>Emulator Team</h4>"
                "<ul>"
                "<li>Niclas Karlsson</li>"
                "<li>David Kinder</li>"
                "<li>Stefan Meier</li>"
                "<li>Paul David Doherty</li>"
                "</ul>"

                "<h4>Magnetic Scrolls Team</h4>"
                "<ul>"
                "<li>Anita Sinclair</li>"
                "<li>Ken Gordon</li>"
                "<li>Hugh Steers</li>"
                "<li>Rob Steggles</li>"
                "</ul>"
        
                "<h4>Writing</h4>"
                "<ul>"
                "<li>Rob Steggles</li>"
                "</ul>"
        
                "<h4>Art</h4>"

                "<ul>"
                // these were later
                //"<li>Allan Hunnisett</li>"
                //"<li>Chris Kent</li>"
                "<li>Geoff Quilley</li>"
                "<li>Tristan Humphries</li>"
                "</ul>"

                "<h4>Music</h4>"
                "<ul>"
                "<li>John Molloy</li>"
                "</ul>"
        
                "<h4>Testing</h4>"
                "<ul>"
                "<li>Servan Keondjian</li>"
                "<li>Anita Sinclair</li>"
                "</ul>"

                "<h4>Special Thanks</h4>"
                "<ul>"
                "<li>Rob Jarratt for DEC tape restoration</li>"
                "<li><p>"
                "Contributors to the <a href=\\\"https://strandgames.com/community\\\">Strand forum</a> and others who've helped with feedback, suggestions and testing."
                "</li></p>"                
                "</ul>"

                "<h4>Additional Credit</h4>"
                "<ul>"
                "<li>" "<p>"
                "Retro PC fonts from <a href=\\\"http://int10h.org/oldschool-pc-fonts\\\">int10.org</a>."
                "</p>" "</li>"
                "</ul>"
        
                "<h3>Original Platforms</h3>"
                "<p>"
                "Amiga, Amstrad CPC, Amstrad PCW, Apple2, Archimedes, Atari ST, Atari XL/XE, Commodore 128/ 64, IBM PC, Macintosh, Sinclair QL, Spectrum 128K, Spectrum +3"
                "</p>";
        

            gs.append(c);
            if (!originalCredits.empty())
            {
                gs.append("<h3>Original Game Credits</h3>");

                const char* p = originalCredits.c_str();
                gs.append("<p>\\\"");
                while (*p)
                {
                    // remove any newlines (json doesn't like them)
                    char c  = *p++;
                    if (c == '\n') c = ' ';
                    gs.add(c);
                }
                gs.append("\\\"</p>");
            }
            gs.add(0);
            build.productCredits(gs.start());

            // add theme colours

            const char* colname = "indigo";
            build.productThemePrimeColor(MatCol::get(colname).toString());
            build.productThemeContrastColor(MatCol::get(MatCol::getContrastName(colname)).toString());
            
            // cover page
            build.productCoverTextColor("black");
            build.productCoverTextFont("Kanit Light");
            //build.productCoverTextWeight(300);
            build.productCoverEffect("River"); 
            //build.productCoverEffect("Oceanic");

            // add features common to magnetic games
            buildCommonProductInfo(build);
        }
        break;
    default:
        LOG("WARNING: ProductInfo, unknown game ", get_game());
        break;
    }
    
    build.endProduct();
    buf.add(0);  // terminate string
}
#endif


#ifdef STANDALONE

void PuzzleManager::text(const char* s)
{
    // emit text to game
    printf("%s\n", s);
}

bool PuzzleManager::action(const char* s)
{
    printf("action: %s\n", s);
    return true;
}

void PuzzleManager::buildProductInfoJSON(GrowString& buf,
                                         const string& originalCredits)
{
    buf.add(0);
}

#endif // STANDALONE

