#include <genesis.h>
#include <ui.h>
#include <synth.h>

int main(void)
{
    VDP_setTextPalette(PAL2);
    VDP_drawText("YM2612 & PSG Test", 11, 0);
    synth_init();
    ui_draw();
    while (TRUE)
    {
        VDP_showFPS(FALSE);
        ui_checkInput();
        VDP_waitVSync();
    }
}
