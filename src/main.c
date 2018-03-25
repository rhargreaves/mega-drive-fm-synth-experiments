#include <genesis.h>
#include <ui.h>
#include <synth.h>
#include <presets.h>

int main(void)
{
    VDP_setTextPalette(PAL2);
    VDP_drawText("Yamaha 2612 Test", 11, 0);
    synth_init();
    synth_preset(&PRESET_CASTLEVANIA);
    ui_init();
    while (TRUE)
    {
        VDP_showFPS(FALSE);
        ui_checkInput();
        VDP_waitVSync();
    }
}
