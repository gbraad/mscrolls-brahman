################################################################################
# Stefan Bylund 2019
#
# Makefile for building a Magnetic game for Sinclair ZX Spectrum Next.
#
# Example: make clean all
################################################################################

GAME := corrupt

GAME_START_IMAGE := 25

GAME_LAST_IMAGE := 25

GAME_VERSION_STR := 'Corruption for ZX Spectrum Next v1.2.0'

MAGNETIC := ../..

CP := cp -r

MKDIR := mkdir -p

RM := rm -rf

ZIP := zip -r -q

ifeq ($(BUILD_TIMEX_HIRES), true)
USE_TIMEX_HIRES := 1
RES := 512
else
USE_TIMEX_HIRES := 0
RES := 256
endif

all:
	$(MAKE) compile BUILD_TIMEX_HIRES=true
	$(MAKE) compile BUILD_TIMEX_HIRES=false
	$(MAKE) distro

compile:
	$(MAKE) -C $(MAGNETIC) clean all CONFIG="-DGAME_FILE=$(GAME).mag -DGAME_DIR=$(GAME) -DGAME_START_IMAGE=$(GAME_START_IMAGE) -DGAME_LAST_IMAGE=$(GAME_LAST_IMAGE) \
	-DGAME_VERSION_STR=$(GAME_VERSION_STR) -DUSE_TIMEX_HIRES=$(USE_TIMEX_HIRES) -DUSE_GFX=1 -DUSE_SOUND=1 -DUSE_MOUSE=1" BUILD_OPT=true
	$(CP) $(MAGNETIC)/bin/magnetic.nex $(GAME)_$(RES).nex

distro:
	$(RM) tmp
	$(MKDIR) tmp/$(GAME)
	$(CP) *.nex tmp/$(GAME)
	$(CP) $(GAME).mag tmp/$(GAME)
	$(CP) gfx tmp/$(GAME)
	$(CP) README.md tmp/$(GAME)
	$(CP) run_cspect.bat tmp/$(GAME)
	$(CP) run_zesarux.bat tmp/$(GAME)
	$(RM) build/$(GAME).zip
	cd tmp; $(ZIP) ../build/$(GAME).zip $(GAME)
	$(RM) tmp

clean:
	$(RM) tmp *.nex
