#
# Makefile to use with emscripten
# See https://emscripten.org/docs/getting_started/downloads.html
# for installation instructions.
#
# This Makefile assumes you have loaded emscripten's environment.
# (On Windows, you may need to execute emsdk_env.bat or encmdprompt.bat ahead)
#
# Running `make` will produce three files:
#  - web/index.html
#  - web/index.js
#  - web/index.wasm
#
# All three are needed to run the demo.

ifdef WINDOWS
CC = gcc
CXX = g++
TARGET = game.exe
else
WEB=1
CC = emcc
CXX = em++
WEB_DIR = web
TARGET = $(WEB_DIR)/index.html
endif

TOP = ../..
IMGUI_DIR = $(TOP)/tools/imgui
KLDIR = $(TOP)/strands
IFIDIR := $(TOP)/ifi
KLSRC = $(KLDIR)/head.cpp $(KLDIR)/list.cpp $(KLDIR)/sat.cpp $(KLDIR)/tree.cpp $(KLDIR)/kl.cpp $(KLDIR)/blob.cpp $(KLDIR)/parsectx.cpp $(KLDIR)/klstrandi.cpp $(KLDIR)/porter2.c $(KLDIR)/strands.cpp  $(IFIDIR)/ifiglue.cpp
SOURCES = main.cpp 
SOURCES += $(IMGUI_DIR)/imgui.cpp $(IMGUI_DIR)/imgui_demo.cpp $(IMGUI_DIR)/imgui_draw.cpp $(IMGUI_DIR)/imgui_tables.cpp $(IMGUI_DIR)/imgui_widgets.cpp $(IMGUI_DIR)/misc/freetype/imgui_freetype.cpp stb_vorbis.c
SOURCES += $(KLSRC)
OBJS = $(addsuffix .o, $(basename $(notdir $(SOURCES))))
UNAME_S := $(shell uname -s)
SOKDIR := $(TOP)/tools/sokol
COMMONDIR := $(TOP)/common
IFIDEFS := -DIFI_BUILD -DIFI_STATIC -DIFI_HANDLE_CHOICE #-DLOGGING
VORBDEFS := -DSTB_VORBIS_NO_PUSHDATA_API -DSTB_VORBIS_MAX_CHANNELS=2 -DSTB_VORBIS_NO_INTEGER_CONVERSION

SOKFLAGS = -I$(SOKDIR) -I$(SOKDIR)/util 
CPPFLAGS = -I.. -I$(IMGUI_DIR) -I$(IFIDIR) -I$(COMMONDIR) -I$(IMGUI_DIR)/backends $(IFIDEFS) $(SOKFLAGS) -I$(IMGUI_DIR)/misc/freetype/include
#CPPFLAGS += -g
CPPFLAGS += -Wall -Wformat -O3 -Wno-switch -fno-rtti -fno-exceptions

##---------------------------------------------------------------------
## EMSCRIPTEN OPTIONS
##---------------------------------------------------------------------

ifdef WEB
SOKFLAGS += -DSOKOL_GLES2
EMS += -s WASM=1
EMS += -s USE_FREETYPE=1
EMS += -s ASYNCIFY=1
EMS += -s ALLOW_MEMORY_GROWTH=1 
EMS += -s "EXPORTED_FUNCTIONS=['_main','_reload_story']"

LDFLAGS += --preload-file ../games/core/init.kl@init.kl
#CPPFLAGS += $(EMS)
LIBS += $(EMS)
LDFLAGS += --shell-file shell.html -O3
endif

ifdef WINDOWS
SOKFLAGS += -DSOKOL_GLCORE33
LDFLAGS += -lkernel32 -luser32 -lshell32 -lgdi32 -lole32
endif

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------

%.o:%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

%.o:$(IMGUI_DIR)/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

%.o:$(IMGUI_DIR)/backends/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

%.o:$(IMGUI_DIR)/misc/freetype/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

%.o:$(KLDIR)/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

%.o:$(KLDIR)/%.c
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

%.o:$(IFIDIR)/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

%.o:../libs/gl3w/GL/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

all:	$(TARGET)
	@echo Build complete for $(TARGET)

$(WEB_DIR):
	mkdir -p $@

serve: all
	python3 -m http.server -d $(WEB_DIR)

ifdef WEB
$(TARGET): $(WEB_DIR) $(OBJS) Makefile
	$(CXX) -o $@ $(OBJS) $(LIBS) $(LDFLAGS)
	mkdir -p $(WEB_DIR)/fonts
	cp fonts/*.ttf $(WEB_DIR)/fonts
	cp -v shell-async.html $(WEB_DIR)/index.html
	cp -v favicon.ico $(WEB_DIR)
else
$(TARGET): $(OBJS) Makefile
	$(CXX) -o $@ $(OBJS) $(LIBS) $(LDFLAGS)
endif

stb_vorbis.o:stb_vorbis.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(VORBDEFS) -c -o $@ $<

clean:
	rm -rf $(TARGET) $(OBJS) $(WEB_DIR) 
