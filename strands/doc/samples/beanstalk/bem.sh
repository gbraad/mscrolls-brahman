export OPT="-std=c++11 -O3 -I../../../../common -DSTANDALONE"
emcc -c ../../../porter2.c
em++ $OPT -o beanstalk.html -s FETCH -s ASYNCIFY --shell-file ../terminal.html ../../../strands.cpp ../../../emcon.cpp porter2.o --preload-file beanstalk.str@story.str



