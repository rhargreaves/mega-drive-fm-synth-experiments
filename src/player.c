#include <genesis.h>
#include <stdbool.h>
#include <util.h>

typedef bool _debouncedFunc(u16 joyState);

static struct FmChannel {
    u8 octave;
    u16 frequency;
    u8 algorithm;
    u8 feedback;
} channel;

static u8 selection = 0;

static void debounce(_debouncedFunc func, u16 joyState, u8 rate);
static void YM2612_setFrequency(u16 freq, u8 octave);
static void YM2612_setAlgorithm(u8 algorithm, u8 feedback);
static void checkPlayNoteButton(u16 joyState);
static bool checkSelectionChangeButtons(u16 joyState);
static bool checkValueChangeButtons(u16 joyState);
static void printValue(const char* header, u16 minSize, u32 value, u16 row);
static void playFmNote(void);
static void stopFmNote(void);

void player_init(void)
{
    channel.frequency = 440;
    channel.octave = 3;
    channel.algorithm = 1;
    channel.feedback = 0;
}

void player_checkInput(void)
{
    u16 joyState = JOY_readJoypad(JOY_1);
    checkPlayNoteButton(joyState);
    debounce(checkSelectionChangeButtons, joyState, 8);
    debounce(checkValueChangeButtons, joyState, 6);

    VDP_setTextPalette(selection == 0 ? PAL3 : PAL0);
    printValue("Frequency", 4, channel.frequency, 3);
    VDP_setTextPalette(selection == 1 ? PAL3 : PAL0);
    printValue("Octave   ", 1, channel.octave, 4);
    VDP_setTextPalette(selection == 2 ? PAL3 : PAL0);
    printValue("Algorithm", 1, channel.algorithm, 5);
    VDP_setTextPalette(selection == 3 ? PAL3 : PAL0);
    printValue("Feedback ", 1, channel.feedback, 6);        
    VDP_setTextPalette(PAL0);
}

static void printValue(const char* header, u16 minSize, u32 value, u16 row)
{
    char text[50];
    char str[5];
    uintToStr(value, str, minSize);
    strcpy(text, header);
    strcat(text, " ");
    strcat(text, str);
    VDP_drawText(text, 0, row);
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

static bool checkSelectionChangeButtons(u16 joyState)
{
    if(joyState & BUTTON_DOWN)
    {
        selection += 1;
    }
    else if(joyState & BUTTON_UP)
    {
        selection -= 1;
    }
    else
    {
        return false;
    }
    if(selection > 3) {
        selection = 0;
    }
    return true;
}

static bool checkValueChangeButtons(u16 joyState)
{
    if(joyState & BUTTON_RIGHT)
    {
        switch(selection) 
        {
            case 1:
                channel.octave++;
                break;
            case 0:
                channel.frequency += 4;
                break;
            case 2:
                channel.algorithm++;
                break;  
            case 3:
                channel.feedback++;
                break;                               
        }
    }
    else if(joyState & BUTTON_LEFT)
    {
        switch(selection) 
        {
            case 1:
                channel.octave--;
                break;
            case 0:
                channel.frequency -= 4;
                break;
            case 2:
                channel.algorithm--;
                break;
            case 3:
                channel.feedback--;
                break;                                
        }
    }
    else
    {
        return false;
    }
    TRUNCATE(channel.feedback, 3);    
    TRUNCATE(channel.algorithm, 3);
    TRUNCATE(channel.octave, 3);
    TRUNCATE(channel.frequency, 11);
    return true;
}

static void debounce(_debouncedFunc func, u16 joyState, u8 rate)
{
    static u16 counter;
    static u16 lastJoyState;
    if(lastJoyState == joyState)
    {
        counter++;
        if(counter > rate)
        {
            counter = 0;
        }
    } 
    else 
    {
        counter = 0;
        lastJoyState = joyState;
    }
    if(counter == 0 && !func(joyState))
    {
        counter = 0;
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
	YM2612_setAlgorithm(channel.algorithm, channel.feedback); // Feedback/algorithm
	YM2612_writeReg(0, 0xB4, 0xC0); // Both speakers on
	YM2612_writeReg(0, 0x28, 0x00); // Key off
    YM2612_setFrequency(channel.frequency, channel.octave);
	YM2612_writeReg(0, 0x28, 0xF0); // Key On
}

static void YM2612_setFrequency(u16 freq /* 11-bit */, u8 octave /* 3-bit */)
{
	YM2612_writeReg(0, 0xA4, (u8)((u16)(freq >> 8) + (u16)(octave << 3)));
	YM2612_writeReg(0, 0xA0, (u8)freq);
}

static void YM2612_setAlgorithm(u8 algorithm /* 3-bit */, u8 feedback /* 3-bit */)
{
	YM2612_writeReg(0, 0xB0, algorithm + (feedback << 3));
}

static void stopFmNote(void)
{
    YM2612_writeReg(0, 0x28, 0x00); // Key Off
}