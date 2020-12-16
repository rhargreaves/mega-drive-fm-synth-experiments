#include <genesis.h>
#include <presets.h>
#include <synth.h>
#include <ui.h>

int main(void)
{
    synth_init();
    synth_preset(&PRESET_CASTLEVANIA);
    ui_init();
    while (TRUE)
    {
        VDP_showFPS(FALSE);
        ui_checkInput();
        SYS_doVBlankProcess();
    }
}
