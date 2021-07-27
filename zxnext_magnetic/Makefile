################################################################################
# Stefan Bylund 2018
#
# Makefile for compiling the Magnetic interpreter for Sinclair ZX Spectrum Next
# using the Z88DK SDCC compiler.
#
# The Magnetic interpreter is configured in the configure.m4 file, which is used
# by this makefile to generate the configuration files (zconfig.h, zconfig.inc,
# zconfig.m4 and zpragma.inc) and the list of source files to compile
# (zproject.lst).
#
# Make rules:
# clean: Remove all generated files.
# all: Compile all source code.
# debug: Compile with debug info.
# config: Only generate configuration files.
# games: Convenience rule for building all games.
#
# Optional make command-line options:
# CONFIG: List of defines passed to configure.m4 to override its configuration.
# BUILD_OPT: Set to true to build with high compiler optimizations.
#
# Example:
# make clean all CONFIG="-DGAME_FILE=pawn.mag -DGAME_DIR=pawn -DGAME_START_IMAGE=4 \
#   -DUSE_TIMEX_HIRES=1 -DUSE_GFX=1 -DUSE_SOUND=1 -DUSE_MOUSE=1"
#   BUILD_OPT=true
################################################################################

CC := zcc

M4 := m4

MKDIR := mkdir -p

RM := rm -rf

CAT := cat

ifeq ($(BUILD_OPT), true)
CFLAGS_OPT := -SO3 --max-allocs-per-node200000
endif

CFLAGS := +zxn -vn -clib=sdcc_iy -Cm"-I src" -Ivt_sound/include $(CFLAGS_OPT)

CFLAGS_DEBUG := --list --c-code-in-asm

LDFLAGS := -m -startup=31 -pragma-include:src/zpragma.inc -subtype=nex -Cz"--main-fence 0xFDFD" -create-app

CFG := src/zconfig.h src/zconfig.inc src/zconfig.m4 src/zpragma.inc zproject.lst

DERIVED := zcc_opt.def zcc_proj.lst src/music_module.asm

INCLUDES := $(wildcard src/*.h vt_sound/include/*.h)

SOURCES := $(shell [ -f zproject.lst ] && $(CAT) zproject.lst)

OBJECTS := $(patsubst %.asm.m4,%.o,$(patsubst %.asm,%.o,$(patsubst %.c,%.o,$(addprefix obj/,$(notdir $(SOURCES))))))

BINARY := bin/magnetic.nex

# This makefile runs in two steps for configuring and then building the project.
# The first step processes the configure.m4 file to generate the configuration
# files (zconfig.h, zconfig.inc, zconfig.m4 and zpragma.inc) and the project
# file (zproject.lst) containing the list of source files to compile.
# The second step will create the OBJECTS variable from the zproject.lst file
# and compile all source code. It is necessary to have two separate steps using
# a recursive make to guarantee that the OBJECTS variable will be correct when
# rebuilding after a configuration change.

.PHONY: all
all: config
	$(MKDIR) obj bin
	$(MAKE) $(BINARY)

.PHONY: debug
debug: config
	$(MKDIR) obj bin
	$(MAKE) DEBUG="$(CFLAGS_DEBUG)" $(BINARY)

.PHONY: config
config: $(CFG)

src/zconfig.h: configure.m4
	$(M4) -DTARGET=1 $(CONFIG) $< > $@

src/zconfig.inc: configure.m4
	$(M4) -DTARGET=2 $(CONFIG) $< > $@

src/zconfig.m4: configure.m4
	$(M4) -DTARGET=3 $(CONFIG) $< > $@

src/zpragma.inc: configure.m4
	$(M4) -DTARGET=4 $(CONFIG) $< > $@

zproject.lst: configure.m4
	$(M4) -DTARGET=5 $(CONFIG) $< > $@

$(BINARY): $(OBJECTS) src/zconfig.m4 src/zpragma.inc
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) -o $(basename $@)

obj/%.o: src/%.c $(INCLUDES)
	$(CC) $(CFLAGS) $(DEBUG) -c -o $@ $<

obj/%.o: src/%.asm.m4 src/zconfig.inc src/zconfig.m4
	$(CC) $(CFLAGS) -c -o $@ $<

obj/%.o: src/%.asm src/zconfig.inc
	$(CC) $(CFLAGS) -c -o $@ $<

obj/%.o: vt_sound/src/%.asm
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: games
games:
	$(MAKE) -C games/pawn    clean all
	$(MAKE) -C games/guild   clean all
	$(MAKE) -C games/jinxter clean all
	$(MAKE) -C games/corrupt clean all
	$(MAKE) -C games/fish    clean all
	$(MAKE) -C games/myth    clean all
	$(MAKE) -C games/wonder  clean all
	$(MAKE) -C games/compilation clean all

.PHONY: clean
clean:
	$(RM) $(CFG) $(DERIVED) obj bin src/*.lis
