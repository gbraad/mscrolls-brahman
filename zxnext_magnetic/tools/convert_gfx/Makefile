################################################################################
# Stefan Bylund 2020
#
# Makefile for compiling the Magnetic Scrolls graphics conversion tool.
################################################################################

MKDIR := mkdir -p

RM := rm -rf

all:
	$(MKDIR) bin
	gcc -O2 -Wall -I../../src -o bin/convert_gfx src/convert_gfx.c

clean:
	$(RM) bin
