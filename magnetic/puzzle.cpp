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
#include "ifmagnetic.h"
#include "ifproduct.h"
#endif

// puzzles for The Pawn
#include "puzpawn.h"

// puzzles for The Guild
#include "puzguild.h"

// puzzles for Jinxter
#include "puzjinxter.h"

// puzzles for Fish
#include "puzfish.h"

// PuzzleManager functions that are ifmagnetic independent
// see also puzman.cpp

void PuzzleManager::start()
{
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
        add(new PouchPuzzle);
        break;
    case 2: // the guild
        add(new JettyPuzzle);
        add(new OldManPuzzle);
        add(new JunkPuzzle);
        add(new CoalPuzzle);
        add(new LampPuzzle);
        add(new PipePuzzle);
        add(new StopcockPuzzle);
        add(new blackboardPuzzle);
        add(new paperbackPuzzle);
        add(new booksPuzzle);
        add(new diaryPuzzle);
        add(new NightSafePuzzle);
        add(new BarsPuzzle);
        add(new BilliardsPuzzle);
        add(new FishPuzzle);
        add(new PoisonPuzzle);
        add(new CubicCagePuzzle);
        add(new WindowPuzzle);
        add(new HorseshoePuzzle);
        add(new LadderPuzzle);
        add(new SandPuzzle);
        add(new ButtonPuzzle);
        add(new MillPuzzle);
        add(new BedPuzzle);
        add(new WaxPuzzle);
        add(new StatuePuzzle);
        add(new noticePuzzle);
        add(new signPuzzle);
        add(new PalmTreePuzzle);
        add(new BoatPuzzle);
        add(new YellowDiePuzzle);
        add(new GreenDiePuzzle);
        add(new BlueDiePuzzle);
        add(new RedDiePuzzle);
        add(new SmallDoorPuzzle);
        add(new GumPuzzle);
        add(new HoorayPuzzle);
        add(new PlasticDiePuzzle);
        add(new LutePuzzle);
        add(new ShorterQueuePuzzle);
        add(new LongerQueuePuzzle);
        //add(new BottlePuzzle);
        add(new PillarPuzzle);
        add(new MachinePuzzle);

        // considering making these a second-level help
        // add(new CoconutPuzzle);
        // add(new SarcophagusPuzzle);
        // add(new FeetPuzzle);
        break;
    case 3: // jinxter
        add(new JdocumentPuzzle);
        add(new JButtonPuzzle);
        add(new JbookPuzzle);
        add(new JPhonePuzzle);
        add(new JFridgePuzzle);
        add(new JMailboxPuzzle);
        add(new JMagpiePuzzle);
        add(new JTrapPuzzle);
        add(new JnotePuzzle);
        add(new JMatchboxPuzzle);
        add(new JMatchPuzzle);
        add(new JletterPuzzle);
        add(new JMilkPuzzle);
        add(new JCanoePuzzle);
        add(new JDirtPuzzle);
        add(new JCanOfWormsPuzzle);
        add(new JAirlockHatchPuzzle);
        add(new JLeftButtonPuzzle);
        add(new JRightButtonPuzzle);
        add(new JWheelPuzzle);
        add(new JnoticePuzzle);
        add(new JChandelierPuzzle);
        add(new JBakerPuzzle);
        add(new JOvenPuzzle);
        add(new JBreadPuzzle);
        add(new JBeerPuzzle);
        add(new JTLHandlePuzzle);
        add(new JTRhandlePuzzle);
        add(new JBLHandlePuzzle);
        add(new JBRHandlePuzzle);
        add(new JClockmakersDoorPuzzle);
        add(new JStoolPuzzle);
        add(new JLadderPuzzle);
        add(new JBeamPuzzle);
        add(new JRainWeathermanPuzzle);
        add(new JHarmonicaPuzzle);
        add(new JBlockPuzzle);
        add(new JUnicornPuzzle);
        add(new JCloudPuzzle);
        add(new JCasePuzzle);
        add(new JPuddlePuzzle);
        add(new JTicketPuzzle);
        add(new JTrainPuzzle);
        add(new JXAMPuzzle);
        add(new JRopePuzzle);
        add(new JWalrusCharmPuzzle);
        add(new JUnicornCharmPuzzle);
        add(new JPelicanCharmPuzzle);
        add(new JInnerHatchPuzzle);
        add(new JCWindowPuzzle);
        add(new JFencePuzzle);
        add(new JTrainRoofPuzzle);
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

std::string PuzzleManager::messageHook(int m, const char* msg)
{
    // NB: game thread

    string cmd;

    // XXXX this is a diabolical hack until i make the code below part
    // of the actual game...
    //if (m > 330) m -= 49;
    
    //LOG3("msg# ", m << " \"" << msg << "\"\n");
    
    switch (get_game())
    {
    case 1: // the pawn
        //if (prog_format) break;
        if (m == 1105) 
        {
            // revealing a path to the nw
            // N/A
            //IItem::getRoom(28).setExit(IItem::dir_nw, 31);
        }
        /*
        if (m == 726)
        {
            LOG3("puz ", "died!, trigger undo");
            if (_triggerUndo)
            {
                // undo wasn't triggered!
                _triggerUndo = false; // give up
            }
            else _triggerUndo = true;
        }
        */
        if (m == 92)
        {
            // halfway down the tower.
            // provide a click link to continue the puzzle
            return has("princess") ? "%s, rescuing the princess" : 0;
        }
        /* // DONE!
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
        */
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
        if (m == 83)
        {
            // DONE!
            
            // narrow track room
            //IItem::getRoom(31).setExit(IItem::dir_n, 32);
        }
        if (m == 1175)
        {
            // bright light
            /*
            if (has("pouch"))
            {
                return "%s. Maybe put [white back](put white in pouch then close pouch) in its pouch";
            }
            */
        }
        /* DONE!
        if (m == 995)
        {
            // magician zaps you back to path

            // reset kronos meet so you get a move before it happens again
            uchar* flagKMeet3 = atKmeet3();
            WRITEW(flagKMeet3, 0);
        }
        */
        if (m == 1161)
        {
            // game end message
        }
        break;
    case 2: // guild

        // WARNING: testing message numbers assumes they don't change.
        // this will happen if any objects are added etc.

        // This has to agree with the version in guild/last/msginfo.i
        // to use MSG(n) find the line number in guild.text, subtract MsgMSGBASE
#define MsgMSGBASE      462
#define MSG(_n)  (MsgMSGBASE-1 + _n)

        if (!_enabled) break; // don't then! :P
        
        if (m == MSG(85)) // too dark to see
        {
            IItem lamp = has("lamp");
            if (lamp)
            {
                GrowString gs;
                IItem cc = closedContainer(lamp);
                if (cc)
                {
                    //LOG3("lamp inside ", cc.toString());
                    gs.append("open ");
                    gs.append(cc.tnWord());
                }

                if (!lamp.isLit())
                {
                    if (!gs.isEmpty()) gs.append(" then ");
                    gs.append("light the Lamp");
                }

                if (!gs.isEmpty())
                {
                    gs.add(0);
                    string s = gs.start();
                    cmd = "%s Maybe [" + s + "](" + s + ").";
                }
            }
        }
        break;
    case 3: // jinxter
        break;
    }

    //if (!cmd.empty()) LOG3("message hook cmd: ", cmd);
      
    return cmd;
}

std::string PuzzleManager::evalUseXwithYSpecial(IItem xi, IItem yi, bool& done)
{
    // set `done` to abort passing command forward.
    string cmd;

    switch (get_game())
    {
    case 1: // pawn
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
    case 2: // Guild
        {
            if (xi.isMoney())
            {
                // drag coin to gate not slot, make it work
                if (xi.wordIs("coin") && yi.wordIs("barrier"))
                {
                    cmd = "put " + xi.toString() + " in slot"; 
                }
                else if (yi.wordIs("gatekeeper"))
                {
                    // if ratrace in progress
                    if (has("grey rat"))
                    {
                        text("Try betting on a specific rat.");
                        done = true;
                    }
                }
                else if (yi.wordIs("rat"))
                {
                    // if race in progress
                    if (has("grey rat"))
                        cmd = "bet " + xi.toString() + " on " + yi.toString();
                }

                if (!done && cmd.empty() && (yi.isNPC() && !yi.wordIs("teller")))
                {
                    // thieves do not give their money away!!
                    text("You decide not to give your money away.");
                    done = true;
                }
            }
            else if (yi.wordIs("button") && (xi.isHard() && xi.size() > 2))
            {
                // generic press button with something
                cmd = "press " + yi.toString() + " with " + xi.toString();
            }

            else if (xi.wordIs("spade") && yi.wordIs("grave"))
            {
                cmd = "dig " + yi.toString() + " with " + xi.toString();
            }
            else if (xi.wordIs("succulents") && yi.wordIs("feet"))
            {
                cmd = "rub " + xi.toString() + " on " + yi.toString();
            }
            else if (xi.hardness() == 11 && (yi.isHard() && !yi.isSurface()))
            {
                // hit coconut with cue/pick/spade
                // hit mineral with cue/pick/spade
                // hit billiard ball with cue etc
                cmd = "hit " + yi.toString() + " with " + xi.toString();
            }
            else if (xi.wordIs("bone") && yi.isLocked())
            {
                cmd = "pick " + yi.toString() + " with " + xi.toString() + " then open it";
            }
            else if ((xi.wordIs("bird") && yi.wordIs("cage")) ||
                     (xi.wordIs("cage") && yi.wordIs("bird")))
            {
                if (has("mynah bird") && has("bird cage"))
                {
                   cmd = "put mynah bird in bird cage";
                }
            }
            else if (xi.wordIs("card") && yi.wordIs("teller"))
            {
                if (has("plastic card")) cmd = "show plastic card to teller";
            }
            else if ((xi.wordIs("die") && yi.wordIs("anticube")) || (xi.wordIs("anticube") && yi.wordIs("die")))
            {
                IItem pd = has("plastic die");
                IItem ac = has("anticube");
                if (pd && ac)
                {
                    pd.setRelatedTo(ac);
                   text("Carefully you staple the anticube onto the die and try to keeping it balanced. ");
                   done = true;
                }
            }
            else if ((xi.wordIs("die") || xi.wordIs("anticube")) && yi.wordIs("machine"))
            {
                IItem pd = has("plastic die");
                IItem ac = has("anticube");
                IItem wm = has("weighing machine");
                if (pd && ac && wm && pd.isRelatedTo(ac.id()))
                {
                    pd.setCarried(); // will reset related to
                    cmd = "put plastic die and anticube on weighing machine";
                }
            }
        }
        break;
    case 3: // jinxter
        {
            if (xi.wordIs("tablecloth") && yi.wordIs("bull"))
            {
                cmd = "wave tablecloth";
            }
            else if ((xi.wordIs("candle") && yi.wordIs("match")) || (xi.wordIs("match") && yi.wordIs("candle")))
            {
                IItem wc = has("wax candle");
                IItem mt = has("match");
                if (wc && mt)
                {
                    cmd = "light candle with match";
                }
            }
            else if ((xi.wordIs("key") && yi.wordIs("candle")) || (xi.wordIs("candle") && yi.wordIs("key")))
            {
                IItem fo = has(xi.toString());
                IItem so = has(yi.toString());
                if (fo && so)
                {
                    if (xi.wordIs("key"))
                       cmd = "heat " + xi.toString() + " with " + yi.toString();
                    else
                       cmd = "heat " + yi.toString() + " with " + xi.toString();
                }
            }
            else if (xi.wordIs("candle") && yi.wordIs("runners"))
            {
                IItem fo = has(xi.toString());
                IItem so = has(yi.toString());
                if (fo && so)
                {
                    cmd = "rub candle on runners";
                }
            }
            else if ((xi.wordIs("worms") && yi.wordIs("mound of dirt")))
            {
                IItem fo = has(xi.toString());
                IItem so = has(yi.toString());
                if (fo && so)
                {
                    cmd = "put worms on mound of dirt";
                }
            }
            else if ((xi.wordIs("worms") && yi.wordIs("can of worms")))
            {
                IItem fo = has(xi.toString());
                IItem so = has(yi.toString());
                if (fo && so)
                {
                    cmd = "put worms in can of worms";
                }
            }
            else if ((xi.wordIs("bottle of milk") && yi.wordIs("mound of dirt")))
            {
                IItem fo = has(xi.toString());
                IItem so = has(yi.toString());
                if (fo && so)
                {
                    cmd = "empty bottle of milk on mound of dirt";
                }
            }
            else if ((xi.wordIs("cylinder") && yi.wordIs("aqualung")))
            {
                IItem cy = has(xi.toString());
                IItem aq = has(yi.toString());
                if (cy && aq)
                {
                    cmd = "fix cylinder to aqualung";
                }
            }
            else if ((xi.wordIs("sieve") && yi.wordIs("flour")))
            {
                IItem si = has(xi.toString());
                if (si)
                {
                    cmd = "sieve flour with sieve";
                }
            }
            else if (xi.wordIs("spell"))
            {
                IItem sp = has(xi.toString());
                if (sp)
                {
                     if (sp.adjWordIf()=="Watchercallit Spell")
                        cmd = "watchercallit "+yi.toString();
                     else if (sp.adjWordIf()=="Oojimy Spell")
                        cmd = "oojimy "+yi.toString();
                     else if (sp.adjWordIf()=="Doofer Spell")
                        cmd = "doofer "+yi.toString();
                }
            }
            else if ((xi.wordIs("saddle") && yi.wordIs("postmistress")))
            {
                IItem saddle = has("saddle");
                if (saddle)
                {
                    cmd = "send saddle to station";
                }
            }
            else if ((xi.wordIs("mouse") && yi.wordIs("postmistress")))
            {
                IItem mouse = has("mouse");
                if (mouse)
                {
                    cmd = "show mouse to postmistress";
                }
            }
            else if ((xi.wordIs("tin") && yi.wordIs("lamp")))
            {
                IItem tin = has("baking tin");
                if (tin)
                {
                    cmd = "throw baking tin at oil lamp";
                }
            }
            else if ((xi.wordIs("bottle of milk") && yi.wordIs("lamp")))
            {
                IItem tin = has("bottle of milk");
                if (tin)
                {
                    cmd = "throw bottle of milk at oil lamp";
                }
            }
            // restrict to the items that are not required anymore
            else if (((xi.wordIs("opener") || xi.wordIs("secateurs") || xi.wordIs("tin")) && yi.wordIs("lamp")))
            {
                 cmd = "throw "+ xi.toString() +" at oil lamp";
            }
            else if ((xi.wordIs("saddle") && yi.wordIs("unicorn")))
            {
                IItem saddle = has("saddle");
                if (saddle)
                {
                    cmd = "put saddle on unicorn";
                }
            }
            else if ((xi.wordIs("glass") && yi.wordIs("rope")))
            {
                cmd = "put beer glass under the rope";
            }
            else if ((xi.wordIs("hat") && yi.wordIs("rope")))
            {
                cmd = "put hat under the rope";
            }
        }
        break;
    }

    if (!cmd.empty())
    {
        LOG3("special command; ", cmd);
    }
    
    return cmd;
}

void PuzzleManager::applyGameFixes()
{
    int g = get_game();
    if (!g) return;
    
    switch (g)
    {
    case 1: // pawn
        {
            LOG3("Puzzman, ", "applying game fixes");

            //IItem("wooden workbench").setMoveable(); // so auto look under works

            // you could get the fridge!!
            //IItem("fridge").setWeight(15);

            // exits from the void
            IItem::getRoom(4).setExit(IItem::dir_n, 3);
            IItem::getRoom(4).setExit(IItem::dir_ne, 16);
            IItem::getRoom(4).setExit(IItem::dir_nw, 6);

            // damp passage with poster make lit
            IItem::getRoom(54).setLit();
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
        case 1: // pawn
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
            else if (ii.wordIs("rice")) s = "Long Grain Rice";
            else if (ii.wordIs("chit")) s = "IOU Chit";
            else if (ii.wordIs("coin")) s = "Ferg Coin";
            break;
    }

    // default
    if (s.empty()) s = ii.toString();
    return s;
}

void PuzzleManager::moveUpdate(int moveCount)
{
    switch(get_game())
    {
    case 1: // pawn
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
    case 2: // Guild
        if (get_current_room() == RNBLACKSQUARE)
        {
            // when you reach the back square, mark all squares as
            // visible on the map.
            for (int i = RNBLACKSQUARE; i <= RNSQUAREDEATH; ++i)
            {
                IItem ri = IItem::getRoom(i);
                assert(ri);
                ri.setVisibleOnMap();
            }
        }
        break;
    case 3: // jinxter
        break;
    }
}

bool PuzzleManager::allowSuggestOpen(IItem ii)
{
    bool r = true; // default to allow
    switch (get_game())
    {
    case 1: // pawn
        break;
    case 2: // guild
        if (ii == IItem("bird cage")) r = false;
        else if (ii == IItem("champagne bottle")) r = false;
        else if (ii == IItem("plastic bag")) r = false;
        break;
    case 3: // jinxter
        break;
    }
    return r;
}

bool PuzzleManager::allowSuggestGet(IItem ii)
{
    bool r = true; // default to allow
    switch (get_game())
    {
    case 1: // pawn
        break;
    case 2: // guild
        if (ii == IItem("ivory cube")) r = false;
        break;
    case 3: // jinxter
        break;
    }
    return r;
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
    case 1: // pawn
        {
            build.productTitle("The Pawn");
            build.productAuthor("by Magnetic Scrolls");

            build.productMarketAndroid("market://details?id=com.voidware.brahmanpawn");
            build.productMarketIOS("https://itunes.apple.com/us/app/the-pawn-by-magnetic-scrolls/id1265982535?ls=1&mt=8");

            build.productPrivacyPolicy("https://strandgames.com/legal/thepawn-mobile-privacy.html");

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
                "<li>Duncan Maclean</li>"
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
    case 2: // guild
        {
            build.productTitle("The Guild of Thieves");
            build.productAuthor("by Magnetic Scrolls");

            build.productMarketAndroid("market://details?id=com.voidware.theguild");
            build.productMarketIOS("https://itunes.apple.com/us/app/the-guild-of-thieves/id1341785644?ls=1&mt=8");

            build.productPrivacyPolicy("https://strandgames.com/legal/guild-mobile-privacy.html");

            GrowString gs;
            const char* c = 
                "<h1>The Guild of Thieves<br/><em>by Magnetic Scrolls 1987</em></h1>"
                "<h3>Restored for 2018</h3>"
        
                "<h4>Strand Games Team</h4>"
                "<ul>"
                "<li>Hugh Steers</li>"
                "<li>Stefan Meier</li>"
                
                // put back in when he contributes!
                //"<li>Rob Steggles</li>"
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
                "<li>John Molloy (new music, thanks John!)</li>"
                "</ul>"
        
                "<h4>Testing</h4>"
                "<ul>"
                "<li>Servan Keondjian</li>"
                "<li>Anita Sinclair</li>"
                "</ul>"

                "<h4>Special Thanks</h4>"
                "<ul>"
                "<li>Duncan Maclean</li>"
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

#if 1
            // water ripples against logo
            build.productCoverTextColor("white");
            build.productCoverTextFont("Kanit Light");
            build.productCoverEffect("Ripple"); 
#endif

            // add features common to magnetic games
            buildCommonProductInfo(build);
        }
        break;
    case 3: // jinxter
        {
            build.productTitle("Jinxter");
            build.productAuthor("by Magnetic Scrolls");
            build.productPrivacyPolicy("https://strandgames.com/legal/jinxter-mobile-privacy.html");

            build.productMarketAndroid("market://details?id=com.voidware.jinxter");
            build.productMarketIOS("http://itunes.apple.com/us/app/jinxter/id1440654170?mt=8");

            GrowString gs;
            const char* c = 
                "<h1>Jinxter<br/><em>by Magnetic Scrolls 1987</em></h1>"
                "<h3><em>Revived</em> for 2018</h3>"
        
                "<h4>Strand Games Team</h4>"
                "<ul>"
                "<li>Hugh Steers</li>"
                "<li>Stefan Meier</li>"
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
                "<li>Paul Findley</li>"
                "<li>Doug Rabson</li>"
                "<li>Richard Huddy</li>"
                "<li>Bob Coles</li>"
                "<li>Servan Keondjian</li>"
                "<li>Steve Lacey (RIP)</li>"
                "<li>Mark Taylor</li>"
                //"<li>Phil South</li>" // fish!
                "</ul>"
        
                "<h4>Writing</h4>"
                "<ul>"
                "<li>Georgina Sinclair</li>"
                "<li>Michael Bywater</li>"
                "</ul>"
        
                "<h4>Original Art</h4>"
                "<ul>"
                "<li>Allan Hunnisett</li>"
                "<li>Chris Kent</li>"
                "<li>Geoff Quilley</li>"
                "<li>Tristan Humphries</li>"
                "</ul>"

                "<h4>Remaster Art</h4>"
                "<ul>"
                "<li>Yaro</li>"
                "</ul>"

                "<h4>Music</h4>"
                "<ul>"
                "<li>Ben Supper (remaster) Thanks Ben!</li>"
                "<li>John Molloy (RIP)</li>"
                //"<li>Pete Kemp</li>"
                "</ul>"
        
                "<h4>Testing</h4>"
                "<ul>"
                "<li>Anita Sinclair</li>"
                "<li>Ben Supper</li>"
                "<li>Phil South</li>"
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
                "<li>Duncan Maclean</li>"
                "<li>" "<p>"
                "Retro PC fonts from <a href=\\\"http://int10h.org/oldschool-pc-fonts\\\">int10.org</a>."
                "</p>" "</li>"
                "</ul>"
        
                "<h3>Original Platforms</h3>"
                "<p>"
                "Amiga, Amstrad CPC, Amstrad PCW, Apple2, Archimedes, Atari ST, Atari XL/XE, Commodore 128/ 64, IBM PC, Macintosh, Sinclair QL, Spectrum 128K, Spectrum +3"
                "</p>";
        

            gs.append(c);

            // NB: no original credits for Jinxter

            gs.add(0);
            build.productCredits(gs.start());

            // add theme colours

            const char* colname = "green";
            build.productThemePrimeColor(MatCol::get(colname).toString());
            build.productThemeContrastColor(MatCol::get(MatCol::getContrastName(colname)).toString());
            
            // cover page


            // the plasma blobs
            build.productCoverTextColor("white");
            build.productCoverTextFont("Kanit Thin");
            build.productCoverTextWeight(300);
            build.productCoverEffect("Plasma"); 

            // add features common to magnetic games
            buildCommonProductInfo(build);
        }
        break;
    case 5: // fish
        {
            build.productTitle("Fish");
            build.productAuthor("by Magnetic Scrolls");
            build.productPrivacyPolicy("https://strandgames.com/legal/fish-mobile-privacy.html");

            build.productMarketAndroid("market://details?id=com.voidware.fish");

            // XX FIXME
            build.productMarketIOS("http://itunes.apple.com/us/app/fish/id1440654170?mt=8");

            GrowString gs;
            const char* c =
                "<h1>Fish!<br/><em>by Magnetic Scrolls 1988</em></h1>"
                "<h3><em>Rewarped</em> in 2019</h3>"

                "<h4>Strand Games Team</h4>"
                "<ul>"
                "<li>Hugh Steers</li>"
                "<li>Stefan Meier</li>"
                "<li>Stefan Bylund</li>"
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
                "<li>Paul Findley</li>"
                "<li>Doug Rabson</li>"
                "<li>Richard Huddy</li>"
                "<li>Bob Coles</li>"
                "<li>Servan Keondjian</li>"
                "<li>Steve Lacey (RIP)</li>"
                "<li>Mark Taylor</li>"
                "<li>Phil South</li>"
                "</ul>"

                "<h4>Writing</h4>"
                "<ul>"
                "<li>John Molloy</li>"
                "<li>Peter Kemp</li>"
                "<li>Phil South</li>"
                "<li>Rob Steggles</li>"
                "</ul>"

                "<h4>Original Art</h4>"
                "<ul>"
                "<li>Geoff Quilley</li>"
                "<li>Allan Hunnisett</li>"
                "<li>Chris Kent</li>"
                "<li>Richard Selby</li>"
                "<li>Tristan Humphries</li>"
                "</ul>"

                "<h4>Remaster Art</h4>"
                "<ul>"
                "<li>-</li>"
                "</ul>"

                "<h4>Music</h4>"
                "<ul>"
                "<li>Ben Supper (remaster) Thanks Ben!</li>"
                "<li>John Molloy (RIP)</li>"
                //"<li>Pete Kemp</li>"
                "</ul>"

                "<h4>Testing</h4>"
                "<ul>"
                "<li>Anita Sinclair</li>"
                "<li>Ben Supper</li>"
                "<li>Phil South</li>"
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
                "<li>Duncan Maclean</li>"
                "<li>" "<p>"
                "Retro PC fonts from <a href=\\\"http://int10h.org/oldschool-pc-fonts\\\">int10.org</a>."
                "</p>" "</li>"
                "</ul>"

                "<h3>Original Platforms</h3>"
                "<p>"
                "Amiga, Amstrad CPC, Amstrad PCW, Apple2, Archimedes, Atari ST, Atari XL/XE, Commodore 128/ 64, IBM PC, Macintosh, Sinclair QL, Spectrum 128K, Spectrum +3"
                "</p>";


            gs.append(c);

            // NB: no original credits for Jinxter

            gs.add(0);
            build.productCredits(gs.start());

            // add theme colours

            const char* colname = "blue";
            build.productThemePrimeColor(MatCol::get(colname).toString());
            build.productThemeContrastColor(MatCol::get(MatCol::getContrastName(colname)).toString());

            // cover page


            // the plasma blobs
            build.productCoverTextColor("white");
            build.productCoverTextFont("Kanit Thin");
            build.productCoverTextWeight(300);
            build.productCoverEffect("Fish");

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

void PuzzleManager::enabled(bool v)
{
    if (v != _enabled)
    {
        _enabled = v;
        if (_enabled) reset();
    }

    LOG3("PuzzleManager enabled, ", _enabled);
}

void PuzzleManager::setRemaster(bool v)
{
    int r = set_REMASTER(v ? 1 : 0);
    if (!r)
    {
        LOG1("MS Puzzle; WARNING: remaster mode failed ", v);
    }
    else
    {
        LOG3("MS Puzzle; remaster mode set ", v);
    }
}

void PuzzleManager::evalKL(const char* expr, bool cr)
{
    //LOG3("MS, Eval KL, ", "");

#ifdef USE_KL

    StreamString s;
    s.open(expr);
    ParseContext pctx(&_kl);
    Term t = parseTerm(s, &pctx);
    if (t)
    {
        if (cr) _klOut << std::endl;
        _kl.eval(t, _kl._env); 
        if (cr) _klOut << std::endl;
    }
#endif
}

void PuzzleManager::handleEventKL(int quiet)
{
#ifdef USE_KL
    if (_enabled) 
    {
        //LOG3("MS, ", "quiet");

        // alert if our KL takes longer than 5ms
        TimeAlert qtime("quiet-handler", 5);

        char cmdbuf[128];
        sprintf(cmdbuf, "(event-hook-handler %d)", quiet);
        evalKL(cmdbuf);
    }
#endif 
}

void PuzzleManager::handleRoomsKL()
{
#ifdef USE_KL
    if (_enabled)
    {
        //LOG3("MS, ", "handleRooms");
        evalKL("(handle-rooms");
    }
#endif
}

void PuzzleManager::initKL()
{
#ifdef USE_KL

    LOG2("MS, ", "initKL");
    _klIn._host = this;
    _klOut._host = this;
    _kl.setInput(&_klIn);
    _kl.setOutput(&_klOut);

    _kl._loadFilePrefix = _host->configDir();
    _kl.loadFile("init.kl", _kl._env);
#endif    
}



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

