# The Pawn MAKE file .....
TOP	=	../..
#EQUATES =	$(TOP)/nonalterable
MAIN	=	$(TOP)/nonalterable
#ALT	=	$(TOP)/advdir/alterable
#MACHINE	=	$(TOP)/machines/dev/last
SCENARIO=	$(TOP)/pawn
SCEN	=	$(SCENARIO)/last
TEXT	=	$(SCENARIO)/text
E	=	$(TOP)/equates


O = opt qs

BIN     =       $(TOP)/tools/bin
AS	=	$(BIN)/as68
LNK	=	$(BIN)/lnk

#FLAGS	=	-i $(SCEN),$(EQUATES),$(ALT)
FLAGS	=	-i $(SCEN),$(MAIN)

M	=	$(MAIN)
#I	=	$(EQUATES)
S	=	$(SCEN)
#C	=	$(MACHINE)

# I=include S=scenario C=computer M=main


OMAIN	=	\
newparsersub.o	\
message.o	\
dig.o		\
npcsub.o	\
bioslink.o	\
random.o	\
charin.o	\
charout.o	\
loadbios.o	\
debugbit.o	\
linktable.o	\
decom.o		\
direct.o	\
evalwht.o	\
functinf.o	\
getdata.o	\
getline.o	\
inputfrm.o	\
msgwrite.o	\
printbit.o	\
screen.o	\
spool.o		\
topsubs.o	\
utils.o		\
valid.o		\
verblist.o	\
context.o	\
doall.o		\
nouns1.o	\
nouns2.o	\
nouns3.o	\
parser1.o	\
parser2.o	\
parser3.o	\
parser4.o	\
parsnpc1.o	\
parsnpc2.o	\
parssub1.o	\
parssub2.o	\
prquotes.o	\
relative.o	\
search.o	\
newsearch.o	\
searcher.o	\
decode.o	\
break.o		\
buy.o		\
cover.o		\
cross.o		\
eat.o		\
empty.o		\
exit.o		\
feed.o		\
feel.o		\
fight.o		\
fire.o		\
goget.o		\
hint.o		\
hit.o		\
jump.o		\
knock.o		\
lean.o		\
light.o		\
listen.o	\
look.o		\
lower.o		\
melt.o		\
mix.o		\
movefix.o	\
open.o		\
point.o		\
push.o		\
putinv.o	\
read.o		\
ride.o		\
say.o		\
show.o		\
sit.o		\
smoke.o		\
strangle.o	\
suss.o		\
tear.o		\
throw.o		\
wake.o		\
wear.o		\
verbsub0.o	\
verbsub2.o	\
verbsub4.o	\
wtable.o	\
common.o	\
in.o		\
parserdat.o	\
verbsub6.o \
pictcode.o \
m2pic.o

OMACHINE	=	

OSCENARIO	=	\
init.o		\
adjectiv.o	\
noundat1.o	\
data1.o		\
data2.o		\
data3.o		\
network.o	\
score.o		\
timer.o		\
prspcmd.o	\
setmode.o	\
m2special.o	\
special0.o	\
special2.o	\
special4.o	\
special6.o	\
ask.o		\
charts.o	\
pictures.o	\
m2pawn.o

prog : $(OMAIN) $(OMACHINE) $(OSCENARIO) firstfile.o 
	$(LNK) -tp -l lfile -o prog

firstfile.o : $M/firstfile.asm
	$(AS) $M/firstfile.asm $(FLAGS) -o firstfile.o  $O
dig.o : $M/dig.asm
	$(AS) $M/dig.asm $(FLAGS)  -o dig.o $O 
pictures.o : $S/pictures.asm
	$(AS) $S/pictures.asm $(FLAGS) -o pictures.o $O
m2pawn.o : $S/m2pawn.asm
	$(AS) $S/m2pawn.asm $(FLAGS) -o m2pawn.o $O
network.o : $S/network.asm
	$(AS) $S/network.asm $(FLAGS) -o network.o $O
m2special.o : $S/m2special.asm
	$(AS) $S/m2special.asm $(FLAGS) -o m2special.o $O
special0.o : $S/special0.asm
	$(AS) $S/special0.asm $(FLAGS) -o special0.o $O
special4.o : $S/special4.asm
	$(AS) $S/special4.asm $(FLAGS) -o special4.o $O
special2.o : $S/special2.asm
	$(AS) $S/special2.asm $(FLAGS) -o special2.o $O
special6.o : $S/special6.asm
	$(AS) $S/special6.asm $(FLAGS) -o special6.o $O
adjectiv.o : $S/adjectiv.asm
	$(AS) $S/adjectiv.asm $(FLAGS) -o adjectiv.o $O
noundat1.o : $S/noundat1.asm
	$(AS) $S/noundat1.asm $(FLAGS) -o noundat1.o $O
setmode.o : $S/setmode.asm
	$(AS) $S/setmode.asm $(FLAGS) -o setmode.o $O
init.o : $S/init.asm
	$(AS) $S/init.asm $(FLAGS) -o init.o $O
common.o : $M/common.asm $S/nounsyms.asm $S/miscsyms.asm $S/scendata.asm $S/sets.asm
	$(AS) $M/common.asm $(FLAGS) -o common.o $O
data1.o : $S/data1.asm $S/scenrw.asm
	$(AS) $S/data1.asm $(FLAGS) -o data1.o $O
data2.o : $S/data2.asm 
	$(AS) $S/data2.asm $(FLAGS) -o data2.o $O
data3.o : $S/data3.asm
	$(AS) $S/data3.asm $(FLAGS) -o data3.o $O
bioslink.o : $M/bioslink.asm
	$(AS) $M/bioslink.asm $(FLAGS) -o bioslink.o $O
charin.o : $M/charin.asm
	$(AS) $M/charin.asm $(FLAGS) -o charin.o $O
charout.o : $M/charout.asm
	$(AS) $M/charout.asm $(FLAGS) -o charout.o $O
cout.o : $C/cout.tst
	$(AS) $C/cout.tst $(FLAGS) -o cout.o $O
charutil.o : $C/charutil.tst
	$(AS) $C/charutil.tst $(FLAGS) -o charutil.o $O
linktable.o : $M/linktable.asm
	$(AS) $M/linktable.asm $(FLAGS) -o linktable.o $O
loadbios.o : $M/loadbios.asm
	$(AS) $M/loadbios.asm $(FLAGS) -o loadbios.o $O
debugbit.o : $M/debugbit.asm
	$(AS) $M/debugbit.asm $(FLAGS) -o debugbit.o $O
decom.o : $M/decom.asm
	$(AS) $M/decom.asm $(FLAGS) -o decom.o $O
direct.o : $M/direct.asm
	$(AS) $M/direct.asm $(FLAGS) -o direct.o $O
evalwht.o : $M/evalwht.asm $S/nounequ2.asm
	$(AS) $M/evalwht.asm $(FLAGS) -o evalwht.o $O
functinf.o : $M/functinf.asm
	$(AS) $M/functinf.asm $(FLAGS) -o functinf.o $O
getdata.o : $M/getdata.asm $S/nounequ2.asm
	$(AS) $M/getdata.asm $(FLAGS) -o getdata.o $O
getline.o : $M/getline.asm
	$(AS) $M/getline.asm $(FLAGS) -o getline.o $O
inputfrm.o : $M/inputfrm.asm
	$(AS) $M/inputfrm.asm $(FLAGS) -o inputfrm.o $O
msgwrite.o : $M/msgwrite.asm
	$(AS) $M/msgwrite.asm $(FLAGS) -o msgwrite.o $O

# source dependencies...

ask.o : $S/ask.asm
	$(AS) $S/ask.asm $(FLAGS) -o ask.o $O
break.o : $M/break.asm
	$(AS) $M/break.asm $(FLAGS) -o break.o $O
buy.o : $M/buy.asm
	$(AS) $M/buy.asm $(FLAGS) -o buy.o $O
cover.o : $M/cover.asm
	$(AS) $M/cover.asm $(FLAGS) -o cover.o $O
cross.o : $M/cross.asm
	$(AS) $M/cross.asm $(FLAGS) -o cross.o $O
eat.o : $M/eat.asm
	$(AS) $M/eat.asm $(FLAGS) -o eat.o $O
empty.o : $M/empty.asm
	$(AS) $M/empty.asm $(FLAGS) -o empty.o $O
exit.o : $M/exit.asm
	$(AS) $M/exit.asm $(FLAGS) -o exit.o $O
feed.o : $M/feed.asm
	$(AS) $M/feed.asm $(FLAGS) -o feed.o $O
feel.o : $M/feel.asm
	$(AS) $M/feel.asm $(FLAGS) -o feel.o $O
fight.o : $M/fight.asm
	$(AS) $M/fight.asm $(FLAGS) -o fight.o $O
fire.o : $M/fire.asm
	$(AS) $M/fire.asm $(FLAGS) -o fire.o $O
goget.o : $M/goget.asm
	$(AS) $M/goget.asm $(FLAGS) -o goget.o $O
hint.o : $M/hint.asm
	$(AS) $M/hint.asm $(FLAGS) -o hint.o $O
hit.o : $M/hit.asm
	$(AS) $M/hit.asm $(FLAGS) -o hit.o $O
jump.o : $M/jump.asm
	$(AS) $M/jump.asm $(FLAGS) -o jump.o $O
knock.o : $M/knock.asm
	$(AS) $M/knock.asm $(FLAGS) -o knock.o $O
lean.o : $M/lean.asm
	$(AS) $M/lean.asm $(FLAGS) -o lean.o $O
light.o : $M/light.asm
	$(AS) $M/light.asm $(FLAGS) -o light.o $O
listen.o : $M/listen.asm
	$(AS) $M/listen.asm $(FLAGS) -o listen.o $O
look.o : $M/look.asm
	$(AS) $M/look.asm $(FLAGS) -o look.o $O
lower.o : $M/lower.asm
	$(AS) $M/lower.asm $(FLAGS) -o lower.o $O
melt.o : $M/melt.asm
	$(AS) $M/melt.asm $(FLAGS) -o melt.o $O
mix.o : $M/mix.asm
	$(AS) $M/mix.asm $(FLAGS) -o mix.o $O
movefix.o : $M/movefix.asm
	$(AS) $M/movefix.asm $(FLAGS) -o movefix.o $O
open.o : $M/open.asm
	$(AS) $M/open.asm $(FLAGS) -o open.o $O
point.o : $M/point.asm
	$(AS) $M/point.asm $(FLAGS) -o point.o $O
push.o : $M/push.asm
	$(AS) $M/push.asm $(FLAGS) -o push.o $O
putinv.o : $M/putinv.asm
	$(AS) $M/putinv.asm $(FLAGS) -o putinv.o $O
read.o : $M/read.asm
	$(AS) $M/read.asm $(FLAGS) -o read.o $O
ride.o : $M/ride.asm
	$(AS) $M/ride.asm $(FLAGS) -o ride.o $O
say.o : $M/say.asm
	$(AS) $M/say.asm $(FLAGS) -o say.o $O
score.o : $S/score.asm
	$(AS) $S/score.asm $(FLAGS) -o score.o $O
show.o : $M/show.asm
	$(AS) $M/show.asm $(FLAGS) -o show.o $O
sit.o : $M/sit.asm
	$(AS) $M/sit.asm $(FLAGS) -o sit.o $O
smoke.o : $M/smoke.asm
	$(AS) $M/smoke.asm $(FLAGS) -o smoke.o $O
strangle.o : $M/strangle.asm
	$(AS) $M/strangle.asm $(FLAGS) -o strangle.o $O
suss.o : $M/suss.asm
	$(AS) $M/suss.asm $(FLAGS) -o suss.o $O
tear.o : $M/tear.asm
	$(AS) $M/tear.asm $(FLAGS) -o tear.o $O
throw.o : $M/throw.asm
	$(AS) $M/throw.asm $(FLAGS) -o throw.o $O
wake.o : $M/wake.asm
	$(AS) $M/wake.asm $(FLAGS) -o wake.o $O
wear.o : $M/wear.asm
	$(AS) $M/wear.asm $(FLAGS) -o wear.o $O

#source dependencies

verbsub0.o : $M/verbsub0.asm
	$(AS) $M/verbsub0.asm $(FLAGS) -o verbsub0.o $O
verbsub2.o : $M/verbsub2.asm $S/nounequ2.asm $S/msginfo.i
	$(AS) $M/verbsub2.asm $(FLAGS) -o verbsub2.o $O
verbsub4.o : $M/verbsub4.asm
	$(AS) $M/verbsub4.asm $(FLAGS) -o verbsub4.o $O
verbsub6.o : $M/verbsub6.asm $S/nounequ2.asm
	$(AS) $M/verbsub6.asm $(FLAGS) -o verbsub6.o $O
pictcode.o : $M/pictcode.asm 
	$(AS) $M/pictcode.asm $(FLAGS) -o pictcode.o $O
m2pic.o : $M/m2pic.asm 
	$(AS) $M/m2pic.asm $(FLAGS) -o m2pic.o $O

#source dependencies

charts.o : $S/charts.asm
	$(AS) $S/charts.asm $(FLAGS) -o charts.o $O
npcsub.o : $M/npcsub.asm
	$(AS) $M/npcsub.asm $(FLAGS) -o npcsub.o $O
prspcmd.o : $S/prspcmd.asm
	$(AS) $S/prspcmd.asm $(FLAGS) -o prspcmd.o $O


# source file dependencies


context.o : $M/context.asm
	$(AS) $M/context.asm $(FLAGS) -o context.o $O
doall.o : $M/doall.asm
	$(AS) $M/doall.asm $(FLAGS) -o doall.o $O
nouns1.o : $M/nouns1.asm
	$(AS) $M/nouns1.asm $(FLAGS) -o nouns1.o $O
nouns2.o : $M/nouns2.asm
	$(AS) $M/nouns2.asm $(FLAGS) -o nouns2.o $O
nouns3.o : $M/nouns3.asm
	$(AS) $M/nouns3.asm $(FLAGS) -o nouns3.o $O
parserdat.o : $M/parserdat.asm
	$(AS) $M/parserdat.asm $(FLAGS) -o parserdat.o $O
in.o : $M/in.asm
	$(AS) $M/in.asm $(FLAGS) -o in.o $O
parser1.o : $M/parser1.asm
	$(AS) $M/parser1.asm $(FLAGS) -o parser1.o $O
parser2.o : $M/parser2.asm
	$(AS) $M/parser2.asm $(FLAGS) -o parser2.o $O
parser3.o : $M/parser3.asm
	$(AS) $M/parser3.asm $(FLAGS) -o parser3.o $O
parser4.o : $M/parser4.asm
	$(AS) $M/parser4.asm $(FLAGS) -o parser4.o $O
parsnpc1.o : $M/parsnpc1.asm
	$(AS) $M/parsnpc1.asm $(FLAGS) -o parsnpc1.o $O
parsnpc2.o : $M/parsnpc2.asm
	$(AS) $M/parsnpc2.asm $(FLAGS) -o parsnpc2.o $O
parssub1.o : $M/parssub1.asm
	$(AS) $M/parssub1.asm $(FLAGS) -o parssub1.o $O
parssub2.o : $M/parssub2.asm
	$(AS) $M/parssub2.asm $(FLAGS) -o parssub2.o $O
prquotes.o : $M/prquotes.asm
	$(AS) $M/prquotes.asm $(FLAGS) -o prquotes.o $O
relative.o : $M/relative.asm
	$(AS) $M/relative.asm $(FLAGS) -o relative.o $O
newsearch.o : $M/newsearch.asm
	$(AS) $M/newsearch.asm $(FLAGS) -o newsearch.o $O
search.o : $M/search.asm
	$(AS) $M/search.asm $(FLAGS) -o search.o $O
decode.o : $M/decode.asm
	$(AS) $M/decode.asm $(FLAGS) -o decode.o $O
searcher.o : $M/searcher.asm
	$(AS) $M/searcher.asm $(FLAGS) -o searcher.o $O
wtable.o : $M/wtable1.asm $S/wtable5.asm $S/wtable6.asm $M/wtable7.asm $S/wtable11noun.asm $S/wtable11misc.asm $S/wtable18.asm $S/wtable4.asm
	$(AS) $M/wtable.asm $(FLAGS) -o wtable.o $O
printbit.o : $M/printbit.asm
	$(AS) $M/printbit.asm $(FLAGS) -o printbit.o $O
random.o : $M/random.asm
	$(AS) $M/random.asm $(FLAGS) -o random.o $O
screen.o : $M/screen.asm
	$(AS) $M/screen.asm $(FLAGS) -o screen.o $O
spool.o : $M/spool.asm
	$(AS) $M/spool.asm $(FLAGS) -o spool.o $O
timer.o : $S/timer.asm
	$(AS) $S/timer.asm $(FLAGS) -o timer.o $O
topsubs.o : $M/topsubs.asm
	$(AS) $M/topsubs.asm $(FLAGS) -o topsubs.o $O
utils.o : $M/utils.asm
	$(AS) $M/utils.asm $(FLAGS) -o utils.o $O
valid.o : $M/valid.asm
	$(AS) $M/valid.asm $(FLAGS) -o valid.o $O
verblist.o : $M/verblist.asm $S/verbequ.asm
	$(AS) $M/verblist.asm $(FLAGS) -o verblist.o $O
message.o : $M/message.asm
	$(AS) $M/message.asm $(FLAGS) -o message.o $O
newparsersub.o : $M/newparsersub.asm
	$(AS) $M/newparsersub.asm $(FLAGS) -o newparsersub.o $O
