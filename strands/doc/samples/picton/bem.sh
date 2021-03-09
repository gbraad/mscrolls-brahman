export OPT="-std=c++11 -O3 -I../../../../common -DSTANDALONE -DNDEBUG"
emcc -c -O3 ../../../porter2.c
em++ $OPT -o picton.html -s FETCH -s ASYNCIFY --shell-file ../terminal.html ../../../strands.cpp ../../../emcon.cpp porter2.o --preload-file story.str@story.str --preload-file core.str@core.str --preload-file map.str@map.str --preload-file people.str@people.str --preload-file picton.str@picton.str



