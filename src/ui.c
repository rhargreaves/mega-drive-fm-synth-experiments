#include <genesis.h>
#include <stdbool.h>
#include <synth.h>

#define OPERATOR_VALUE_COLUMN 11
#define OPERATOR_VALUE_WIDTH 6
#define OPERATOR_TOP_ROW 14
#define SELECTION_COUNT FM_PARAMETER_COUNT + (OPERATOR_PARAMETER_COUNT * OPERATOR_COUNT)

typedef void _changeValueFunc();
typedef void _debouncedFunc(u16 joyState);

typedef struct
{
    const char name[10];
    const u16 minSize;
    const u8 step;
    FmParameter *fmParameter;
} FmParameterUi;

static FmParameterUi fmParameterUis[] = {
    {"Glob LFO ", 1, 1},
    {"LFO Freq ", 1, 1},
    {"Note     ", 2, 1},
    {"Freq Num ", 4, 4},
    {"Octave   ", 1, 1},
    {"Algorithm", 1, 1},
    {"Feedback ", 1, 1},
    {"LFO AMS  ", 1, 1},
    {"LFO FMS  ", 1, 1},
    {"Stereo   ", 1, 1}};

static void updateUiIfRequired(void);
static void requestUiUpdate(void);
static void debounce(_debouncedFunc func, u16 joyState);
static void checkPlayNoteButton(u16 joyState);
static void checkSelectionChangeButtons(u16 joyState);
static void checkValueChangeButtons(u16 joyState);
static void printNumber(u16 number, u16 minSize, u16 x, u16 y);
static void printNote(u16 index, u16 row);
static void printOnOff(u16 index, u16 row);
static void printLFOFreq(u16 index, u16 row);
static void printLookup(u16 index, const char *text, u16 row);
static void printFmParameter(u16 value, u16 minSize, u16 index, u16 row);
static void updateOpParameter(u16 joyState);
static void updateFmParameter(u16 joyState);
static void printFmParameters(void);
static void printOperators(void);
static void printOperator(Operator *op);
static void printOperatorHeader(Operator *op);
static void printStereo(u16 index, u16 row);
static void printAlgorithm(u16 index, u16 row);
static void printAms(u16 index, u16 row);
static void printFms(u16 index, u16 row);

static u8 selection = 0;
static bool drawUi = false;

void ui_init(void)
{
    for (int i = 0; i < FM_PARAMETER_COUNT; i++)
    {
        fmParameterUis[i].fmParameter = synth_fmParameter(i);
    }
}

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
        FmParameterUi *p = &fmParameterUis[index];
        VDP_setTextPalette(PAL2);
        VDP_drawText(p->name, 0, row);
        VDP_setTextPalette(selection == index ? PAL3 : PAL0);
        printFmParameter(p->fmParameter->value, p->minSize, index, row);
    }
}

static void printFmParameter(u16 value, u16 minSize, u16 index, u16 row)
{
    if (index == PARAMETER_NOTE)
    {
        printNote(value, row);
    }
    else if (index == PARAMETER_G_LFO_FREQ)
    {
        printLFOFreq(value, row);
    }
    else if (index == PARAMETER_G_LFO_ON)
    {
        printOnOff(value, row);
    }
    else if (index == PARAMETER_STEREO)
    {
        printStereo(value, row);
    }
    else if (index == PARAMETER_ALGORITHM)
    {
        printAlgorithm(value, row);
    }
    else if (index == PARAMETER_LFO_AMS)
    {
        printAms(value, row);
    }
    else if (index == PARAMETER_LFO_FMS)
    {
        printFms(value, row);
    }
    else
    {
        printNumber(value,
                    minSize,
                    10,
                    row);
    }
}

static void printLFOFreq(u16 index, u16 row)
{
    const char TEXT[][8] = {"3.98Hz", "5.56Hz", "6.02Hz", "6.37Hz", "6.88Hz", "9.63Hz", "48.1Hz", "72.2Hz"};
    printLookup(index, TEXT[index], row);
}

static void printOnOff(u16 index, u16 row)
{
    const char TEXT[][4] = {"Off", "On"};
    printLookup(index, TEXT[index], row);
}

static void printNote(u16 index, u16 row)
{
    const char TEXT[][3] = {"B", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#"};
    printLookup(index, TEXT[index], row);
}

static void printStereo(u16 index, u16 row)
{
    const char TEXT[][4] = {"Off", "R", "L", "LR"};
    printLookup(index, TEXT[index], row);
}

static void printAlgorithm(u16 index, u16 row)
{
    const char TEXT[][8] = {"1-2-3-4", "1/2-3-4", "1/23-4", "12/3-4", "1/3-2/4", "1-2/3/4", "12/3/4", "1/2/3/4"};
    printLookup(index, TEXT[index], row);
}

static void printAms(u16 index, u16 row)
{
    const char TEXT[][7] = {"0", "1.4dB", "5.9dB", "11.8dB"};
    printLookup(index, TEXT[index], row);
}

static void printFms(u16 index, u16 row)
{
    const char TEXT[][4] = {"0", "3.4", "6.7", "10", "14", "20", "40", "80"};
    printLookup(index, TEXT[index], row);
}

static void printLookup(u16 index, const char *text, u16 row)
{
    char buffer[25];
    sprintf(buffer, "%s (%u)     ", text, index);
    VDP_drawText(buffer, 10, row);
}

static void printOperators(void)
{
    for (u16 opIndex = 0; opIndex < OPERATOR_COUNT; opIndex++)
    {
        Operator *op = synth_operator(opIndex);
        printOperatorHeader(op);
        printOperator(op);
    }
}

static void printOperatorHeader(Operator *op)
{
    VDP_setTextPalette(PAL2);
    char opHeader[4];
    sprintf(opHeader, "Op%u", op->opNumber + 1);
    VDP_drawText(opHeader, OPERATOR_VALUE_WIDTH * op->opNumber + OPERATOR_VALUE_COLUMN, OPERATOR_TOP_ROW);
    VDP_setTextPalette(PAL0);
}

static void printOperator(Operator *op)
{
    for (u16 index = 0; index < OPERATOR_PARAMETER_COUNT; index++)
    {
        u16 row = index + OPERATOR_TOP_ROW + 1;
        if (op->opNumber == 0)
        {
            VDP_setTextPalette(PAL2);
            VDP_drawText(operator_parameterName(op, index), 0, row);
            VDP_setTextPalette(PAL0);
        }
        if (selection - FM_PARAMETER_COUNT == index + op->opNumber * OPERATOR_PARAMETER_COUNT)
        {
            VDP_setTextPalette(PAL3);
        }
        printNumber(operator_parameterValue(op, index),
                    operator_parameterMinSize(op, index),
                    OPERATOR_VALUE_WIDTH * op->opNumber + OPERATOR_VALUE_COLUMN,
                    row);
        VDP_setTextPalette(PAL0);
    }
}

static void printNumber(u16 number, u16 minSize, u16 x, u16 y)
{
    char str[5];
    uintToStr(number, str, minSize);
    VDP_drawText(str, x, y);
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
    FmParameterUi *uiParameter = &fmParameterUis[selection];
    FmParameter *parameter = uiParameter->fmParameter;
    if (joyState & BUTTON_RIGHT)
    {
        parameter->value += uiParameter->step;
    }
    else if (joyState & BUTTON_LEFT)
    {
        parameter->value -= uiParameter->step;
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
    u16 opParaIndex = selection - FM_PARAMETER_COUNT;
    Operator *op = synth_operator(opParaIndex / OPERATOR_PARAMETER_COUNT);
    OpParameters opParameter = opParaIndex % OPERATOR_PARAMETER_COUNT;
    u16 value = operator_parameterValue(op, opParameter);
    u16 step = operator_parameterStep(op, opParameter);
    if (joyState & BUTTON_RIGHT)
    {
        value += step;
    }
    else if (joyState & BUTTON_LEFT)
    {
        value -= step;
    }
    else
    {
        return;
    }
    operator_setParameterValue(op, opParameter, value);
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
