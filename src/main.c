#include <genesis.h>
#include <ui.h>
#include <synth.h>

int main(void)
{
    VDP_setTextPalette(PAL0);
    VDP_drawText("YM2612 & PSG Test", 11, 0);
    synth_init();
    while (TRUE)
    {
        VDP_showFPS(FALSE);
        ui_checkInput();
        VDP_waitVSync();
    }
}
