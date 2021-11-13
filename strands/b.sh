export OPT="-std=c++11 -I../common -DSTANDALONE -DLOGGING"
gcc -c porter2.c
g++ $OPT -o strands strands.cpp porter2.o
