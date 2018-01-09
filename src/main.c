#include <genesis.h>

static void vintEvent();

int main()
{
    VDP_setTextPalette(PAL1);
    VDP_drawText("FM & PSG Test", 13, 0);
    SYS_setVIntCallback(vintEvent);

    PlayFmNote();
    PlayChord();

    char str[40];
    int frame = 0;
	while(1)
    {
        VDP_waitVSync();
        frame++;
        uintToStr(frame, str, 1);
        strcat(str, "th Frame");
        VDP_drawText(str, 14, 1);
    }
    return (0);
}

void PlayFmNote()
{
    YM2612_writeReg(0, 0x22, 0);    // LFO Off
	YM2612_writeReg(0, 0x27, 0);    // Ch 3 Normal
	YM2612_writeReg(0, 0x28, 0);    // All channels off
	YM2612_writeReg(0, 0x28, 1);
	YM2612_writeReg(0, 0x28, 2);
	YM2612_writeReg(0, 0x28, 4);
	YM2612_writeReg(0, 0x28, 5);
	YM2612_writeReg(0, 0x28, 6);
	YM2612_writeReg(0, 0x30, 0x71); // DT1/MUL
	YM2612_writeReg(0, 0x34, 0x0D);
	YM2612_writeReg(0, 0x38, 0x33);
	YM2612_writeReg(0, 0x3C, 0x01);
	YM2612_writeReg(0, 0x40, 0x23); // Total Level
	YM2612_writeReg(0, 0x44, 0x2D);
	YM2612_writeReg(0, 0x48, 0x26);
	YM2612_writeReg(0, 0x4C, 0x00);
	YM2612_writeReg(0, 0x50, 0x5F); // RS/AR
	YM2612_writeReg(0, 0x54, 0x99);
	YM2612_writeReg(0, 0x58, 0x5F);
	YM2612_writeReg(0, 0x5C, 0x99);
	YM2612_writeReg(0, 0x60, 5);   // AM/D1R
	YM2612_writeReg(0, 0x64, 5);
	YM2612_writeReg(0, 0x68, 5);
	YM2612_writeReg(0, 0x6C, 7);
	YM2612_writeReg(0, 0x70, 2);   // D2R
	YM2612_writeReg(0, 0x74, 2);
	YM2612_writeReg(0, 0x78, 2);
	YM2612_writeReg(0, 0x7C, 2);
	YM2612_writeReg(0, 0x80, 0x11);    // D1L/RR
	YM2612_writeReg(0, 0x84, 0x11);
	YM2612_writeReg(0, 0x88, 0x11);
	YM2612_writeReg(0, 0x8C, 0xA6);
	YM2612_writeReg(0, 0x90, 0);   // Proprietary
	YM2612_writeReg(0, 0x94, 0);
	YM2612_writeReg(0, 0x98, 0);
	YM2612_writeReg(0, 0x9C, 0);
	YM2612_writeReg(0, 0xB0, 0x32); // Feedback/algorithm
	YM2612_writeReg(0, 0xB4, 0xC0); // Both speakers on
	YM2612_writeReg(0, 0x28, 0x00); // Key off
	YM2612_writeReg(0, 0xA4, 0x22); // Set Freq
	YM2612_writeReg(0, 0xA0, 0x69);
	YM2612_writeReg(0, 0x28, 0xF0); // Key On
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

static void vintEvent()
{
}
