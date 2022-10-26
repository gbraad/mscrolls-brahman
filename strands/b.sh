export OPT="-std=c++11 -I../common -DSTANDALONE -DLOGGING -static-libgcc -static-libstdc++ -O2"
gcc -c porter2.c
g++ $OPT -c strands.cpp head.cpp list.cpp tree.cpp blob.cpp parsectx.cpp klstrandi.cpp
g++ $OPT -o strands strands.o porter2.o head.o list.o tree.o blob.o parsectx.o klstrandi.o
