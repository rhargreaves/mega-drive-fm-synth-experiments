#include <genesis.h>

int main()
{
    VDP_setTextPalette(PAL1);
    VDP_drawText("FM & PSG Test", 13, 0);

    PlayChord();

	while(1)
    {
        VDP_waitVSync();
    }
    return (0);
}

void PlayChord()
{
    u16 FREQ_A = 440;
    u16 FREQ_C_SHARP = 554;
    u16 FREQ_E = 659;

    PSG_setFrequency(0, FREQ_A);
    PSG_setEnvelope(0, 1);
    PSG_setFrequency(1, FREQ_C_SHARP);
    PSG_setEnvelope(1, 1);
    PSG_setFrequency(2, FREQ_E);
    PSG_setEnvelope(2, 1);
}
