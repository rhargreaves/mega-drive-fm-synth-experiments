#include <genesis.h>
#include <ui.h>
#include <synth.h>

static void printFrame();
static void vintEvent();

static u16 frameNumber = 0;

int main(void)
{
    VDP_setTextPalette(PAL0);
    VDP_drawText("YM2612 & PSG Test", 11, 0);
    SYS_setVIntCallback(vintEvent);
    synth_init();
    while (TRUE)
    {
        printFrame();
        ui_checkInput();
        VDP_waitVSync();
    }
}

static void printFrame(void)
{
    char text[9] = "Frame ";
    char str[3];
    uintToStr(frameNumber, str, 2);
    strcat(text, str);
    VDP_drawText(text, 30, 0);
}

static void vintEvent(void)
{
    if (++frameNumber == (IS_PALSYSTEM ? 50 : 60))
    {
        frameNumber = 0;
    }
}
