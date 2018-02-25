#include <genesis.h>
#include <player.h>

static void printFrame();
static void vintEvent();

static u32 frame = 0;

int main(void)
{
    VDP_setTextPalette(PAL0);
    VDP_drawText("YM2612 & PSG Test", 11, 0);
    SYS_setVIntCallback(vintEvent);

	while(TRUE)
    {
        printFrame();
        playJoy();
        VDP_waitVSync();
    }
    return (0);
}

static void printFrame(void)
{
    char text[9] = "Frame ";
    char str[3];
    uintToStr(frame, str, 2);
    strcat(text, str);
    VDP_drawText(text, 30, 0);
}

static void vintEvent(void)
{
    if(++frame == (IS_PALSYSTEM ? 50 : 60))
    {
        frame = 0;
    }
}
