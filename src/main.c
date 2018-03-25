#include <genesis.h>
#include <ui.h>
#include <synth.h>
#include <presets.h>

int main(void)
{
    VDP_setPalette(PAL0, font_pal_lib.data);
    VDP_setPaletteColor((PAL1 * 16) + 15, 0x0C55);
    VDP_setPaletteColor((PAL2 * 16) + 15, 0x00DE);
    VDP_setTextPalette(PAL2);
    VDP_drawText("Yamaha 2612 Test", 11, 0);
    synth_init();
    synth_preset(&PRESET_CASTLEVANIA_2);
    ui_init();
    while (TRUE)
    {
        VDP_showFPS(FALSE);
        ui_checkInput();
        VDP_waitVSync();
    }
}
