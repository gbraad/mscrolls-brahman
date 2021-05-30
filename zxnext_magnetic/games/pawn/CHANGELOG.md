# Changelog

All notable changes to The Pawn game will be documented in this file.

## 1.3.0 (2021-05-23)

* Recompiled with z88dk v2.1 using the new Magnetic interpreter optimisations.
* Added missing space character after load/save filename prompt.
* Changed the quit message to be more clear about the choice of quitting or
restarting the game.
* Make sure the game starts in verbose mode when restarted.
* Make "graphics on" command take immediate effect.
* Adjusted digit '1' in the proportional font so that all digits are monospaced.
* Fixed a text scrolling issue when going from a location without an image to a
location with an image and that movement triggers text output that is partially
hidden behind the new location image.

## 1.2.0 (2020-09-04)

* Improved font for Timex hi-res mode.
* Smoother location image transitions.

## 1.1.0 (2020-08-04)

* Game story file pawn.prg is trimmed from unused symbols and relocation
information to significantly reduce its loading time.

## 1.0.0 (2020-07-18)

* First public release.
