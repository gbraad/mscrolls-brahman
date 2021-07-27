@echo off

REM Batch file for building Magnetic on Windows without make.

REM Configuration

REM Configuration for Timex hi-res mode 512x192
set CONFIG=-DFAST_COMPILE -DUSE_PROG_FORMAT=0 -DUSE_GFX=1 -DUSE_TIMEX_HIRES=1 -DUSE_SOUND=0 -DUSE_MOUSE=1

REM Configuration for Spectrum mode 256x192
REM set CONFIG=-DFAST_COMPILE -DUSE_PROG_FORMAT=0 -DUSE_GFX=1 -DUSE_TIMEX_HIRES=0 -DUSE_SOUND=1 -DUSE_MOUSE=1

m4 -DTARGET=1 %CONFIG% configure.m4 > src/zconfig.h
m4 -DTARGET=2 %CONFIG% configure.m4 > src/zconfig.inc
m4 -DTARGET=3 %CONFIG% configure.m4 > src/zconfig.m4
m4 -DTARGET=4 %CONFIG% configure.m4 > src/zpragma.inc
m4 -DTARGET=5 %CONFIG% configure.m4 > zproject.lst

REM Compilation

REM set SDCC_OPT=-SO3 --max-allocs-per-node200000

@echo on

zcc +zxn -vn -m -startup=31 -clib=sdcc_iy %SDCC_OPT% -Cm"-I src" -Ivt_sound/include -pragma-include:src/zpragma.inc @zproject.lst -o bin/magnetic -subtype=nex -Cz"--main-fence 0xFDFD" -create-app
