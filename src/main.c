#include <genesis.h>

int main()
{
    VDP_drawText("FM & PSG Test", 13, 0);

	while(1)
    {
            VDP_waitVSync();
    }
    return (0);
}
