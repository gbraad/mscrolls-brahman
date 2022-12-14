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

# comment this out to remove spine
#USESPINE=1
#USEDEMO=1

ifdef WINDOWS
CC = gcc
CXX = g++
TARGET = game.exe
else
WEB=1
CC = emcc
CXX = em++ -std=c++17
WEB_DIR = web
TARGET = $(WEB_DIR)/index.html
endif

TOP = ../..
IMGUI_DIR = $(TOP)/tools/imgui
KLDIR = $(TOP)/strands
SRCDIR = $(KLDIR)/imgui
IFIDIR := $(TOP)/ifi
KLSRC = $(KLDIR)/head.cpp $(KLDIR)/list.cpp $(KLDIR)/sat.cpp $(KLDIR)/tree.cpp $(KLDIR)/kl.cpp $(KLDIR)/blob.cpp $(KLDIR)/parsectx.cpp $(KLDIR)/klstrandi.cpp $(KLDIR)/porter2.c $(KLDIR)/strands.cpp  $(IFIDIR)/ifiglue.cpp
SOURCES = $(SRCDIR)/main.cpp 
SOURCES += $(IMGUI_DIR)/imgui.cpp $(IMGUI_DIR)/imgui_draw.cpp $(IMGUI_DIR)/imgui_tables.cpp $(IMGUI_DIR)/imgui_widgets.cpp $(IMGUI_DIR)/misc/freetype/imgui_freetype.cpp $(SRCDIR)/stb_vorbis.c $(SRCDIR)/imwebp.cpp
SOURCES += $(KLSRC)
UNAME_S := $(shell uname -s)
SOKDIR := $(TOP)/tools/sokol
COMMONDIR := $(TOP)/common
IFIDEFS := -DIFI_BUILD -DIFI_STATIC -DIFI_HANDLE_CHOICE #-DLOGGING
VORBDEFS := -DSTB_VORBIS_NO_PUSHDATA_API -DSTB_VORBIS_MAX_CHANNELS=2 -DSTB_VORBIS_NO_INTEGER_CONVERSION

WEBP_DIR = $(TOP)/tools/libwebp

WEBPSRC = \
    $(WEBP_DIR)/src/dec/alpha_dec.c \
    $(WEBP_DIR)/src/dec/buffer_dec.c \
    $(WEBP_DIR)/src/dec/frame_dec.c \
    $(WEBP_DIR)/src/dec/idec_dec.c \
    $(WEBP_DIR)/src/dec/io_dec.c \
    $(WEBP_DIR)/src/dec/quant_dec.c \
    $(WEBP_DIR)/src/dec/tree_dec.c \
    $(WEBP_DIR)/src/dec/vp8_dec.c \
    $(WEBP_DIR)/src/dec/vp8l_dec.c \
    $(WEBP_DIR)/src/dec/webp_dec.c \
	\
    $(WEBP_DIR)/src/dsp/alpha_processing.c \
    $(WEBP_DIR)/src/dsp/cpu.c \
    $(WEBP_DIR)/src/dsp/dec.c \
    $(WEBP_DIR)/src/dsp/dec_clip_tables.c \
    $(WEBP_DIR)/src/dsp/filters.c \
    $(WEBP_DIR)/src/dsp/lossless.c \
    $(WEBP_DIR)/src/dsp/rescaler.c \
    $(WEBP_DIR)/src/dsp/upsampling.c \
    $(WEBP_DIR)/src/dsp/yuv.c \
	\
    $(WEBP_DIR)/src/utils/bit_reader_utils.c \
    $(WEBP_DIR)/src/utils/color_cache_utils.c \
    $(WEBP_DIR)/src/utils/filters_utils.c \
    $(WEBP_DIR)/src/utils/huffman_utils.c \
    $(WEBP_DIR)/src/utils/quant_levels_dec_utils.c \
    $(WEBP_DIR)/src/utils/random_utils.c \
    $(WEBP_DIR)/src/utils/rescaler_utils.c \
    $(WEBP_DIR)/src/utils/thread_utils.c \
    $(WEBP_DIR)/src/utils/utils.c \


SOURCES += $(WEBPSRC)
WEBPDEFS += -I$(WEBP_DIR)


SOKFLAGS = -I$(SOKDIR) -I$(SOKDIR)/util 
CPPFLAGS = -I.. -I$(IMGUI_DIR) -I$(IFIDIR) -I$(COMMONDIR) -I$(IMGUI_DIR)/backends $(IFIDEFS) $(SOKFLAGS) -I$(IMGUI_DIR)/misc/freetype/include $(WEBPDEFS)
#CPPFLAGS += -g

SPINEDIR = $(TOP)/tools/spine-cpp
SPINE = $(SPINEDIR)/src/spine
SPINESRC =  \
    sspine.cpp \
    $(SPINE)/Animation.cpp \
    $(SPINE)/AnimationState.cpp \
    $(SPINE)/AnimationStateData.cpp \
    $(SPINE)/Atlas.cpp \
    $(SPINE)/AtlasAttachmentLoader.cpp \
    $(SPINE)/Attachment.cpp \
    $(SPINE)/AttachmentLoader.cpp \
    $(SPINE)/AttachmentTimeline.cpp \
    $(SPINE)/Bone.cpp \
    $(SPINE)/BoneData.cpp \
    $(SPINE)/BoundingBoxAttachment.cpp \
    $(SPINE)/ClippingAttachment.cpp \
    $(SPINE)/ColorTimeline.cpp \
    $(SPINE)/ConstraintData.cpp \
    $(SPINE)/CurveTimeline.cpp \
    $(SPINE)/DeformTimeline.cpp \
    $(SPINE)/DrawOrderTimeline.cpp \
    $(SPINE)/Event.cpp \
    $(SPINE)/EventData.cpp \
    $(SPINE)/EventTimeline.cpp \
    $(SPINE)/Extension.cpp \
    $(SPINE)/IkConstraint.cpp \
    $(SPINE)/IkConstraintData.cpp \
    $(SPINE)/IkConstraintTimeline.cpp \
    $(SPINE)/Json.cpp \
    $(SPINE)/LinkedMesh.cpp \
    $(SPINE)/MathUtil.cpp \
    $(SPINE)/MeshAttachment.cpp \
    $(SPINE)/PathAttachment.cpp \
    $(SPINE)/PathConstraint.cpp \
    $(SPINE)/PathConstraintData.cpp \
    $(SPINE)/PathConstraintMixTimeline.cpp \
    $(SPINE)/PathConstraintPositionTimeline.cpp \
    $(SPINE)/PathConstraintSpacingTimeline.cpp \
    $(SPINE)/PointAttachment.cpp \
    $(SPINE)/RTTI.cpp \
    $(SPINE)/RegionAttachment.cpp \
    $(SPINE)/RotateTimeline.cpp \
    $(SPINE)/ScaleTimeline.cpp \
    $(SPINE)/ShearTimeline.cpp \
    $(SPINE)/Skeleton.cpp \
    $(SPINE)/SkeletonBinary.cpp \
    $(SPINE)/SkeletonBounds.cpp \
    $(SPINE)/SkeletonClipping.cpp \
    $(SPINE)/SkeletonData.cpp \
    $(SPINE)/SkeletonJson.cpp \
    $(SPINE)/Skin.cpp \
    $(SPINE)/Slot.cpp \
    $(SPINE)/SlotData.cpp \
    $(SPINE)/SpineObject.cpp \
    $(SPINE)/TextureLoader.cpp \
    $(SPINE)/Timeline.cpp \
    $(SPINE)/TransformConstraint.cpp \
    $(SPINE)/TransformConstraintData.cpp \
    $(SPINE)/TransformConstraintTimeline.cpp \
    $(SPINE)/TranslateTimeline.cpp \
    $(SPINE)/Triangulator.cpp \
    $(SPINE)/Log.cpp \
    $(SPINE)/Updatable.cpp \
    $(SPINE)/VertexAttachment.cpp \
    $(SPINE)/VertexEffect.cpp

ifdef USESPINE
SOURCES += $(SPINESRC)
CPPFLAGS += -I$(SPINEDIR)/include -DUSESPINE
endif

ifdef USEDEMO
SOURCES += $(IMGUI_DIR)/imgui_demo.cpp
CPPFLAGS += -DUSEDEMO
endif

OBJS = $(addsuffix .o, $(basename $(notdir $(SOURCES))))
OPTFLAGS = -Os
CPPFLAGS += -Wall -Wformat -O2 -Wno-switch -fno-rtti -fno-exceptions $(OPTFLAGS)

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

LDFLAGS += -lidbfs.js
LDFLAGS += --preload-file ../games/core/init.kl@init.kl
#CPPFLAGS += $(EMS)
LIBS += $(EMS) --pre-js $(SRCDIR)/pre.js
LDFLAGS += --shell-file $(SRCDIR)/shell.html $(OPTFLAGS)
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

%.o:$(SPINE)/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

%.o:$(SRCDIR)/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

%.o:$(WEBP_DIR)/src/dec/%.c
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

%.o:$(WEBP_DIR)/src/dsp/%.c
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

%.o:$(WEBP_DIR)/src/utils/%.c
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

all:	$(TARGET)
	@echo Build complete for $(TARGET)

$(WEB_DIR):
	mkdir -p $@
	mkdir -p $(WEB_DIR)/fonts
	mkdir -p $(WEB_DIR)/audio
	mkdir -p $(WEB_DIR)/images

$(WEB_DIR)/fontlist.txt: $(WEB_DIR)
	ls -1 $(SRCDIR)/fonts > $(WEB_DIR)/fontlist.txt

serve: all
	python3 -m http.server -d $(WEB_DIR)

ifdef WEB
$(TARGET): $(WEB_DIR) $(OBJS) Makefile $(WEB_DIR)/fontlist.txt
	$(CXX) -o $@ $(OBJS) $(LIBS) $(LDFLAGS)
	cp -v $(SRCDIR)/fonts/*.ttf $(WEB_DIR)/fonts
	cp -v $(SRCDIR)/favicon.ico $(WEB_DIR)
	cp -v $(SRCDIR)/shell-async.html $(WEB_DIR)/index.html
else
$(TARGET): $(OBJS) Makefile
	$(CXX) -o $@ $(OBJS) $(LIBS) $(LDFLAGS)
endif

stb_vorbis.o:$(SRCDIR)/stb_vorbis.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(VORBDEFS) -c -o $@ $<

clean:
	rm -rf $(TARGET) $(WEB_DIR) *.tmp *.o *~
