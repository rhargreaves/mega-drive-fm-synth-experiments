#include <genesis.h>
#include <stdbool.h>
#include <synth.h>

typedef void _changeValueFunc();
typedef void _debouncedFunc(u16 joyState);

static void debounce(_debouncedFunc func, u16 joyState, u8 rate);

static void checkPlayNoteButton(u16 joyState);
static void checkSelectionChangeButtons(u16 joyState);
static void checkValueChangeButtons(u16 joyState);
static void printValue(const char *header, u16 minSize, u16 value, u16 row);
static void printText(const char *header, u16 minSize, const char *value, u16 row);

static const char notes_text[][3] = {"B ", "C ", "C#", "D ", "D#", "E ", "F ", "F#", "G ", "G#", "A ", "A#"};

static u8 selection = 0;

void player_checkInput(void)
{
    u16 joyState = JOY_readJoypad(JOY_1);
    checkPlayNoteButton(joyState);
    debounce(checkSelectionChangeButtons, joyState, 8);
    debounce(checkValueChangeButtons, joyState, 6);

    for(u16 index = 0; index < maxFmParameters(); index++)
    {
        FmParameter *p = fmParameter(index);
        VDP_setTextPalette(selection == index ? PAL3 : PAL0);
        if(index == PARAMETER_NOTE)
        {
            const char *note_text = notes_text[p->value];
            printText(p->name, p->minSize, note_text, index + 3);
        }
        else
        {
            printValue(p->name, p->minSize, p->value, index + 3);
        }
    }
    VDP_setTextPalette(PAL0);
}

static void printText(const char *header, u16 minSize, const char *value, u16 row)
{
    char text[50];
    strcpy(text, header);
    strcat(text, " ");
    strcat(text, value);
    VDP_drawText(text, 0, row);
}

static void printValue(const char *header, u16 minSize, u16 value, u16 row)
{
    char str[5];
    uintToStr(value, str, minSize);
    printText(header, minSize, str, row);
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

static void checkSelectionChangeButtons(u16 joyState)
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
        return;
    }
    u16 maxParameters = maxFmParameters();
    if(selection == (u8)-1) {
        selection = maxParameters - 1;
    }
    if(selection > maxParameters - 1) {
        selection = 0;
    }
}

static void checkValueChangeButtons(u16 joyState)
{
    FmParameter *parameter = fmParameter(selection);
    if(joyState & BUTTON_RIGHT)
    {
        parameter->value += parameter->step;
    }
    else if(joyState & BUTTON_LEFT)
    {
        parameter->value -= parameter->step;
    }
    else
    {
        return;
    }
    if(parameter->value == (u16)-1) {
        parameter->value = parameter->maxValue;
    }
    if(parameter->value > parameter->maxValue) {
        parameter->value = 0;
    }
    parameter->onUpdate();
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
    if(counter == 0)
    {
        func(joyState);
    }
}
