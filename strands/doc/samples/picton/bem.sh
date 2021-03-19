set -x # echo
export OPT="-std=c++11 -O3 -I../../../../common -DSTANDALONE -DNDEBUG -Wno-switch"
emcc -c -O3 ../../../porter2.c
rm -rf picton
mkdir picton
em++ $OPT -o picton/index.html -s FETCH -s ASYNCIFY --shell-file ../terminal.html ../../../strands.cpp ../../../emcon.cpp porter2.o --preload-file story.str@story.str --preload-file core.str@core.str --preload-file map.str@map.str --preload-file picton.str@picton.str
rm -f picton/*.tmp*
cp ../*.js picton
cp ../*.css picton
cp *.css picton



