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

CC = emcc
CXX = em++
WEB_DIR = web
EXE = $(WEB_DIR)/index.html
IMGUI_DIR = ../../tools/imgui
KLDIR = ..
IFIDIR := ../../ifi
KLSRC = $(KLDIR)/head.cpp $(KLDIR)/list.cpp $(KLDIR)/sat.cpp $(KLDIR)/tree.cpp $(KLDIR)/kl.cpp $(KLDIR)/blob.cpp $(KLDIR)/parsectx.cpp $(KLDIR)/klstrandi.cpp $(KLDIR)/porter2.c $(KLDIR)/strands.cpp  $(IFIDIR)/ifiglue.cpp
SOURCES = sdl.cpp imgui_freetype.cpp
SOURCES += $(IMGUI_DIR)/imgui.cpp $(IMGUI_DIR)/imgui_demo.cpp $(IMGUI_DIR)/imgui_draw.cpp $(IMGUI_DIR)/imgui_tables.cpp $(IMGUI_DIR)/imgui_widgets.cpp
SOURCES += $(IMGUI_DIR)/backends/imgui_impl_sdl.cpp $(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp
SOURCES += $(KLSRC)
OBJS = $(addsuffix .o, $(basename $(notdir $(SOURCES))))
UNAME_S := $(shell uname -s)
USE_FILE_SYSTEM := 1

## just headers here
FREETYPE := freetype

COMMONDIR := ../../common
IFIDEFS := -DIFI_BUILD -DIFI_STATIC -DIFI_HANDLE_CHOICE -s ASYNCIFY=1 #-DLOGGING

GAMEDIR := ../doc/samples/picton
GAME := --preload-file $(GAMEDIR)/story.str@story.str --preload-file $(GAMEDIR)/core.str@core.str --preload-file $(GAMEDIR)/map.str@map.str --preload-file $(GAMEDIR)/picton.str@picton.str --preload-file $(GAMEDIR)/init.kl@init.kl

##---------------------------------------------------------------------
## EMSCRIPTEN OPTIONS
##---------------------------------------------------------------------

EMS += -s USE_SDL=2 -s WASM=1
#EMS += -s ALLOW_MEMORY_GROWTH=1
#EMS += -s DISABLE_EXCEPTION_CATCHING=1 -s NO_EXIT_RUNTIME=0
#EMS += -s ASSERTIONS=1
EMS += -s USE_FREETYPE=1


LDFLAGS += --no-heap-copy --preload-file fonts@/fonts $(GAME) -s ASYNCIFY=1


##---------------------------------------------------------------------
## FINAL BUILD FLAGS
##---------------------------------------------------------------------

CPPFLAGS = -I.. -I$(IMGUI_DIR) -I$(IFIDIR) -I$(COMMONDIR) -I$(IMGUI_DIR)/backends -I$(FREETYPE)/include $(IFIDEFS)
#CPPFLAGS += -g
CPPFLAGS += -Wall -Wformat -O3 -Wno-switch
CPPFLAGS += $(EMS)
LIBS += $(EMS)
LDFLAGS += --shell-file shell_minimal.html

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------

%.o:%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

%.o:$(IMGUI_DIR)/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

%.o:$(IMGUI_DIR)/backends/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

%.o:$(KLDIR)/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

%.o:$(KLDIR)/%.c
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

%.o:$(IFIDIR)/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<


%.o:../libs/gl3w/GL/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

all: $(EXE)
	@echo Build complete for $(EXE)

$(WEB_DIR):
	mkdir $@

serve: all
	python3 -m http.server -d $(WEB_DIR)

$(EXE): $(OBJS) $(WEB_DIR)
	$(CXX) -o $@ $(OBJS) $(LIBS) $(LDFLAGS)

clean:
	rm -rf $(OBJS) $(WEB_DIR)
