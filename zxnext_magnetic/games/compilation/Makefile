################################################################################
# Stefan Bylund 2020
#
# Makefile for building the Magnetic Scrolls Compilation for Sinclair
# ZX Spectrum Next.
#
# Example: make clean all
################################################################################

GAMES := magnetic

SPUI := resources/SPUI-0.2.1.zip

CP := cp -r

MKDIR := mkdir -p

RM := rm -rf

UNZIP := unzip -q

ZIP := zip -r -q

all: games games_boot

.PHONY: games
games:
	$(RM) tmp
	$(MKDIR) tmp/$(GAMES)
	$(UNZIP) ../pawn/build/pawn.zip -d tmp/$(GAMES)
	$(UNZIP) ../guild/build/guild.zip -d tmp/$(GAMES)
	$(UNZIP) ../jinxter/build/jinxter.zip -d tmp/$(GAMES)
	$(UNZIP) ../corrupt/build/corrupt.zip -d tmp/$(GAMES)
	$(UNZIP) ../fish/build/fish.zip -d tmp/$(GAMES)
	$(UNZIP) ../myth/build/myth.zip -d tmp/$(GAMES)
	$(RM) build/$(GAMES).zip
	cd tmp; $(ZIP) ../build/$(GAMES).zip $(GAMES)
	$(RM) tmp

.PHONY: games_boot
games_boot:
	$(RM) tmp
	$(MKDIR) tmp/boot/dot/
	$(MKDIR) tmp/boot/nextzxos/
	$(UNZIP) $(SPUI) -d tmp
	$(CP) tmp/RELEASE-SPUI/SPUI/dot/SPUI tmp/boot/dot/
	$(CP) resources/autoexec.bas tmp/boot/nextzxos/
	$(UNZIP) build/$(GAMES).zip -d tmp/boot
	$(RM) build/$(GAMES)-boot.zip
	cd tmp/boot; $(ZIP) ../../build/$(GAMES)-boot.zip .
	$(RM) tmp

clean:
	$(RM) tmp
