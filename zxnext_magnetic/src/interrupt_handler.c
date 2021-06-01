/*******************************************************************************
 * Stefan Bylund 2020
 *
 * General IM2 interrupt handler that invokes the mouse handler and/or animation
 * module. Also updates a frame counter variable.
 *
 * Note: The file sound.c also contains an interrupt handler that is only used
 * when playing the intro music before the game is started and then unregistered.
 * The file emu.c contains an interrupt handler used by the interpreter logger,
 * which is never used together with the general interrupt handler due to size
 * restrictions.
 ******************************************************************************/

#include <arch/zxn.h>
#include <z80.h>
#include <intrinsic.h>
#include <stdint.h>

#include "zconfig.h"
#include "interrupt_handler.h"
#include "mouse.h"
#include "animation.h"
#include "ide_friendly.h"

#if USE_ANIM
extern void page_in_game(void);

static uint16_t frame_counter = 0;
#endif

IM2_DEFINE_ISR(interrupt_handler)
{
#if USE_MOUSE
    mouse_isr();
#endif

#if USE_ANIM
    if (animation_on && (frame_counter % ANIMATION_FRAME_INTERVAL == 0))
    {
        animate();
        page_in_game();
        // Restore default page 10 in MMU slot 2.
        ZXN_WRITE_MMU2(10);
    }

    frame_counter++;
#endif
}

void init_interrupt_handler(void)
{
#if USE_ANIM
    // Disable original ULA interrupt and enable line interrupt mid-screen at
    // line ANIMATION_START_LINE. In this way, we get more time for drawing the
    // animation step until the next screen frame is displayed.
    ZXN_NEXTREG(REG_LINE_INTERRUPT_CONTROL, 0x06);
    ZXN_NEXTREG(REG_LINE_INTERRUPT_VALUE_L, ANIMATION_START_LINE);
#endif

    // Install general IM2 interrupt service routine.
    intrinsic_di();
    z80_bpoke(0xFDFD, 0xC3); // jp
    z80_wpoke(0xFDFE, (uint16_t) interrupt_handler);
    intrinsic_ei();
}
