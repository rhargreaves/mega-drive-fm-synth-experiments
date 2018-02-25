#include <genesis.h>
#include <stdbool.h>

typedef bool _debouncedFunc(u16 joyState);

static void debounce(_debouncedFunc func, u16 joyState, u8 rate);

static void checkPlayNoteButton(u16 joyState);
static void checkFreqChangeButtons(u16 joyState, u8 rate);
static bool checkFreqChangeButtonsInt(u16 joyState);
static void printFrequency(void);
static void playChord();
static void playFmNote();
static void stopFmNote();

static u8 frequency = 0x22;

void playJoy(void)
{
    u16 joyState = JOY_readJoypad(JOY_1);
    checkPlayNoteButton(joyState);
    debounce(checkFreqChangeButtonsInt, joyState, 5);
    printFrequency();
}

static void printFrequency(void)
{
    char text[50] = "Frequency = ";
    char str[5];
    uintToStr(frequency, str, 3);
    strcat(text, str);
    VDP_drawText(text, 0, 2);
}

static void checkPlayNoteButton(u16 joyState)
{
    static bool playing = false;
    if(joyState & BUTTON_A)
    {
        if(!playing)
        {
            playFmNote();
        }
        playing = true;
    }
    else
    {
        playing = false;
        stopFmNote();
    }
}

static bool checkFreqChangeButtonsInt(u16 joyState)
{
    if(joyState & BUTTON_UP)
    {
        frequency++;
    }
    else if(joyState & BUTTON_DOWN)
    {
        frequency--;
    }
    else
    {
        return false;
    }
    return true;
}

static void debounce(_debouncedFunc func, u16 joyState, u8 rate)
{
    static int changing = 0;
    if(changing == 0)
    {
        bool handled = func(joyState);
        if(!handled)
        {
            changing = 0;
        }
    }
    changing++;
    if(changing > rate)
    {
        changing = 0;
    }
}

static void playFmNote(void)
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
	YM2612_writeReg(0, 0xA4, frequency); // Set Freq
	YM2612_writeReg(0, 0xA0, 0x69);
	YM2612_writeReg(0, 0x28, 0xF0); // Key On
}

static void stopFmNote(void)
{
    YM2612_writeReg(0, 0x28, 0x00); // Key Off
}

static void playChord(void)
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
