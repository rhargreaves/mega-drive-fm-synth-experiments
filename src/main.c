#include <genesis.h>

int main()
{
    VDP_setTextPalette(PAL1);
    VDP_drawText("FM & PSG Test", 13, 0);

    PSG_write(0b10001110);
    PSG_write(0b00001111);
    PSG_setEnvelope(0, 0);

	while(1)
    {
        VDP_waitVSync();
    }
    return (0);
}
