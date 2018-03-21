#include <genesis.h>
#include <stdbool.h>
#include <synth.h>

#define OPERATOR_TOP_ROW 14
#define SELECTION_COUNT FM_PARAMETER_COUNT + (OPERATOR_PARAMETER_COUNT * OPERATOR_COUNT)

typedef void _changeValueFunc();
typedef void _debouncedFunc(u16 joyState);

static void updateUiIfRequired(void);
static void requestUiUpdate(void);
static void debounce(_debouncedFunc func, u16 joyState);
static void checkPlayNoteButton(u16 joyState);
static void checkSelectionChangeButtons(u16 joyState);
static void checkValueChangeButtons(u16 joyState);
static void printValue(const char *header, u16 minSize, u16 value, u16 row);
static void printText(const char *header, const char *value, u16 row);
static void printNumber(u16 number, u16 minSize, u16 x, u16 y);
static void printNote(const char *name, u16 index, u16 row);
static void updateOpParameter(u16 joyState);
static void updateFmParameter(u16 joyState);
static void printFmParameters(void);
static void printOperators(void);
static void printOperator(Operator *op);

static u8 selection = 0;
static bool drawUi = false;

void ui_draw(void)
{
    printFmParameters();
    printOperators();
    VDP_setTextPalette(PAL0);
}

void ui_checkInput(void)
{
    u16 joyState = JOY_readJoypad(JOY_1);
    checkPlayNoteButton(joyState);
    debounce(checkSelectionChangeButtons, joyState);
    debounce(checkValueChangeButtons, joyState);
    updateUiIfRequired();
}

static void printFmParameters(void)
{
    for (u16 index = 0; index < FM_PARAMETER_COUNT; index++)
    {
        const u16 TOP_ROW = 3;
        u16 row = index + TOP_ROW;
        FmParameter *p = synth_fmParameter(index);
        VDP_setTextPalette(selection == index ? PAL3 : PAL0);
        if (index == PARAMETER_NOTE)
        {
            printNote(p->name, p->value, row);
        }
        else
        {
            printValue(p->name, p->minSize, p->value, row);
        }
    }
}

static void printNote(const char *name, u16 index, u16 row)
{
    const char NOTES_TEXT[][3] = {"B ", "C ", "C#", "D ", "D#", "E ", "F ", "F#", "G ", "G#", "A ", "A#"};
    const char *noteText = NOTES_TEXT[index];
    printText(name, noteText, row);
}

static void printOperators(void)
{
    for (u16 opIndex = 0; opIndex < OPERATOR_COUNT; opIndex++)
    {
        Operator *op = synth_operator(opIndex);
        VDP_setTextPalette(PAL2);
        printNumber(op->opNumber, 1, (6 * op->opNumber) + 6, OPERATOR_TOP_ROW);
        printOperator(op);
    }
}

static void printOperator(Operator *op)
{
    for (u16 index = 0; index < OPERATOR_PARAMETER_COUNT; index++)
    {
        VDP_setTextPalette(selection == index + FM_PARAMETER_COUNT ? PAL3 : PAL0);
        u16 row = index + OPERATOR_TOP_ROW + 1;
        OperatorParameter *p = operator_parameter(op, (OpParameters)index);
        if (op->opNumber == 0)
        {
            VDP_drawText(p->name, 0, row);
        }
        printNumber(p->value,
                    p->minSize,
                    (6 * op->opNumber) + 6,
                    row);
    }
}

static void printNumber(u16 number, u16 minSize, u16 x, u16 y)
{
    char str[5];
    uintToStr(number, str, minSize);
    VDP_drawText(str, x, y);
}

static void printText(const char *header, const char *value, u16 row)
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
    printText(header, str, row);
}

static void checkPlayNoteButton(u16 joyState)
{
    static bool playing = false;
    if (joyState & BUTTON_A)
    {
        if (!playing)
        {
            synth_playNote();
        }
        playing = true;
    }
    else
    {
        playing = false;
        synth_stopNote();
    }
}

static void checkSelectionChangeButtons(u16 joyState)
{
    if (joyState & BUTTON_DOWN)
    {
        selection += 1;
    }
    else if (joyState & BUTTON_UP)
    {
        selection -= 1;
    }
    else
    {
        return;
    }
    if (selection == (u8)-1)
    {
        selection = SELECTION_COUNT - 1;
    }
    if (selection > SELECTION_COUNT - 1)
    {
        selection = 0;
    }
    requestUiUpdate();
}

static void checkValueChangeButtons(u16 joyState)
{
    if (selection < FM_PARAMETER_COUNT)
    {
        updateFmParameter(joyState);
    }
    else
    {
        updateOpParameter(joyState);
    }
}

static void updateFmParameter(u16 joyState)
{
    FmParameter *parameter = synth_fmParameter(selection);
    if (joyState & BUTTON_RIGHT)
    {
        parameter->value += parameter->step;
    }
    else if (joyState & BUTTON_LEFT)
    {
        parameter->value -= parameter->step;
    }
    else
    {
        return;
    }
    if (parameter->value == (u16)-1)
    {
        parameter->value = parameter->maxValue;
    }
    if (parameter->value > parameter->maxValue)
    {
        parameter->value = 0;
    }
    parameter->onUpdate();
    requestUiUpdate();
}

static void updateOpParameter(u16 joyState)
{
    OpParameters opParameter = selection - FM_PARAMETER_COUNT;
    Operator *op = synth_operator(0);
    OperatorParameter *parameter = operator_parameter(op, opParameter);
    if (joyState & BUTTON_RIGHT)
    {
        parameter->value += parameter->step;
    }
    else if (joyState & BUTTON_LEFT)
    {
        parameter->value -= parameter->step;
    }
    else
    {
        return;
    }
    if (parameter->value == (u16)-1)
    {
        parameter->value = parameter->maxValue;
    }
    if (parameter->value > parameter->maxValue)
    {
        parameter->value = 0;
    }
    parameter->onUpdate(synth_operator(0));
    requestUiUpdate();
}

static void debounce(_debouncedFunc func, u16 joyState)
{
    const u8 REPEAT_RATE = 2;
    static u16 counter;
    static u16 lastJoyState;
    if (lastJoyState == joyState)
    {
        counter++;
        if (counter > REPEAT_RATE)
        {
            counter = 0;
        }
    }
    else
    {
        counter = 0;
        lastJoyState = joyState;
    }
    if (counter == 0)
    {
        func(joyState);
    }
}

static void requestUiUpdate(void)
{
    drawUi = true;
}

static void updateUiIfRequired(void)
{
    if (drawUi)
    {
        drawUi = false;
        ui_draw();
    }
}
