export OPT="-std=c++11 -I../common -DSTANDALONE -DLOGGING"
gcc -c porter2.c
g++ $OPT -c head.cpp list.cpp tree.cpp blob.cpp parsectx.cpp klstrandi.cpp
g++ $OPT -o strands strands.cpp porter2.o head.o list.o tree.o blob.o parsectx.o klstrandi.o