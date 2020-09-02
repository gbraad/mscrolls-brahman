/*******************************************************************************
 * Stefan Bylund 2018
 *
 * Implementation of sound.h; a limited C API for playing and stopping a PT3
 * music module using the Vortex Tracker II (VT) player.
 ******************************************************************************/

#include <arch/zxn.h>
#include <z80.h>
#include <intrinsic.h>
#include <stdint.h>
#include <stdbool.h>

#include "sound.h"
#include "vt_sound.h"
#include "ide_friendly.h"

extern uint8_t music_module[];

void play_music(void)
{
    // Place the vt_sound binary and music module (located in bank 14 with origin
    // address 0) in MMU slots 0 and 1 for execution.
    // Note: The vt_sound binary (~2.2 KB) and music module must fit within 16 KB.
    ZXN_WRITE_MMU0(28);
    ZXN_WRITE_MMU1(29);

    // Initialize vt_sound with the music module.
    vt_init(music_module);

    // Install the vt_play_isr() interrupt service routine. The music will now
    // play in the background. Note: Pages 28 and 29 must remain in MMU slots
    // 0 and 1 while music is playing and until stop_music() is called.
    intrinsic_di();
    z80_bpoke(0xFDFD, 0xC3); // jp
    z80_wpoke(0xFDFE, (uint16_t) vt_play_isr);
    intrinsic_ei();
}

void stop_music(void)
{
    // Disable the vt_play_isr() interrupt service routine and mute the sound.
    vt_set_play_isr_enabled(false);
    vt_mute();

    // Install an empty interrupt service routine.
    intrinsic_di();
    z80_bpoke(0xFDFD, 0xFB); // ei
    z80_bpoke(0xFDFE, 0xED); // reti
    z80_bpoke(0xFDFF, 0x4D);
    intrinsic_ei();

    // Remove the vt_sound binary and music module by restoring the Spectrum ROM
    // in MMU slots 0 and 1.
    ZXN_WRITE_MMU0(255);
    ZXN_WRITE_MMU1(255);
}
