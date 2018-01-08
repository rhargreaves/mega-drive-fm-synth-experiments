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
    PSG_setFrequency(0, 440);
    PSG_setEnvelope(0, 1);
    PSG_setFrequency(1, 554);
    PSG_setEnvelope(1, 1);
    PSG_setFrequency(2, 659);
    PSG_setEnvelope(2, 1);
}
