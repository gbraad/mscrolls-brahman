export OPT="-std=c++11 -O3 -I../../../../common -DSTANDALONE"
emcc -c -O3 ../../../porter2.c
em++ $OPT -o cod.html -s FETCH -s ASYNCIFY --shell-file ../terminal.html ../../../strands.cpp ../../../emcon.cpp porter2.o --preload-file story.str@story.str --preload-file core.str@core.str --preload-file cod.str@cod.str



