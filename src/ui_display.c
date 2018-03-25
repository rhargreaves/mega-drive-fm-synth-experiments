#include <genesis.h>
#include <synth.h>
#include <channel.h>
#include <stdbool.h>
#include <ui_display.h>

#define OPERATOR_VALUE_COLUMN 11
#define OPERATOR_VALUE_WIDTH 6
#define OPERATOR_TOP_ROW 14

static void printNumber(u16 number, u16 minSize, u16 x, u16 y);
static void printNote(u16 index, u16 row);
static void printOnOff(u16 index, u16 row);
static void printLFOFreq(u16 index, u16 row);
static void printLookup(u16 index, const char *text, u16 row);
static void printGlobalParameters(u8 selection);
static void printFmParameters(Channel *chan, u8 selection);
static void printOperators(Channel *chan, u8 selection);
static void printOperator(Operator *op, u8 selection);
static void printOperatorHeader(Operator *op);
static void printStereo(u16 index, u16 row);
static void printAlgorithm(u16 index, u16 row);
static void printAms(u16 index, u16 row);
static void printFms(u16 index, u16 row);

static FmParameterUi globalParameterUis[] = {
    {"Glob LFO", 1, NULL, printOnOff},
    {"LFO Freq", 1, NULL, printLFOFreq}};

static FmParameterUi fmParameterUis[] = {
    {"Note", 2, NULL, printNote},
    {"Freq Num", 4, NULL, NULL},
    {"Octave", 1, NULL, NULL},
    {"Algorithm", 1, NULL, printAlgorithm},
    {"Feedback", 1, NULL, NULL},
    {"LFO AMS", 1, NULL, printAms},
    {"LFO FMS", 1, NULL, printFms},
    {"Stereo", 1, NULL, printStereo}};

static OperatorParameterUi opParameterUis[] = {
    {"Multiple", 2},
    {"Detune", 1},
    {"Total Lvl", 3},
    {"Rate Scale", 1},
    {"Atck Rate", 2},
    {"Ampl Mode", 1},
    {"1st Decay", 2},
    {"2nd Decay", 2},
    {"Sub Level", 2},
    {"Rel Rate", 2}};

static bool drawUi = false;

void display_draw(Channel *chan, u8 selection)
{
    printGlobalParameters(selection);
    printFmParameters(chan, selection);
    printOperators(chan, selection);
    VDP_setTextPalette(PAL0);
}

void display_requestUiUpdate(void)
{
    drawUi = true;
}

void display_updateUiIfRequired(Channel *chan, u8 selection)
{
    if (drawUi)
    {
        drawUi = false;
        display_draw(chan, selection);
    }
}

static void printGlobalParameters(u8 selection)
{
    for (u16 index = 0; index < GLOBAL_PARAMETER_COUNT; index++)
    {
        const u16 TOP_ROW = 3;
        u16 row = index + TOP_ROW;
        FmParameterUi *p = &globalParameterUis[index];
        VDP_setTextPalette(PAL2);
        VDP_drawText(p->name, 0, row);
        VDP_setTextPalette(selection == index ? PAL3 : PAL0);
        u16 value = synth_globalParameterValue(index);
        if (p->printFunc != NULL)
        {
            p->printFunc(value, row);
        }
        else
        {
            printNumber(value,
                        p->minSize,
                        10,
                        row);
        }
    }
}

static void printFmParameters(Channel *chan, u8 selection)
{
    for (u16 index = 0; index < FM_PARAMETER_COUNT; index++)
    {
        const u16 TOP_ROW = 5;
        u16 row = index + TOP_ROW;
        FmParameterUi *p = &fmParameterUis[index];
        VDP_setTextPalette(PAL2);
        VDP_drawText(p->name, 0, row);
        VDP_setTextPalette(selection == index + GLOBAL_PARAMETER_COUNT ? PAL3 : PAL0);
        u16 value = channel_parameterValue(chan, index);
        if (p->printFunc != NULL)
        {
            p->printFunc(value, row);
        }
        else
        {
            printNumber(value,
                        p->minSize,
                        10,
                        row);
        }
    }
}

static void printOperators(Channel *chan, u8 selection)
{
    for (u16 opIndex = 0; opIndex < OPERATOR_COUNT; opIndex++)
    {
        Operator *op = channel_operator(chan, opIndex);
        printOperatorHeader(op);
        printOperator(op, selection);
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

static void printOperator(Operator *op, u8 selection)
{
    for (u16 index = 0; index < OPERATOR_PARAMETER_COUNT; index++)
    {
        u16 row = index + OPERATOR_TOP_ROW + 1;
        if (op->opNumber == 0)
        {
            VDP_setTextPalette(PAL2);
            VDP_drawText(opParameterUis[index].name, 0, row);
            VDP_setTextPalette(PAL0);
        }
        if (selection - FM_PARAMETER_COUNT == GLOBAL_PARAMETER_COUNT + index + op->opNumber * OPERATOR_PARAMETER_COUNT)
        {
            VDP_setTextPalette(PAL3);
        }
        printNumber(operator_parameterValue(op, index),
                    opParameterUis[index].minSize,
                    OPERATOR_VALUE_WIDTH * op->opNumber + OPERATOR_VALUE_COLUMN,
                    row);
        VDP_setTextPalette(PAL0);
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

static void printNumber(u16 number, u16 minSize, u16 x, u16 y)
{
    char str[5];
    uintToStr(number, str, minSize);
    VDP_drawText(str, x, y);
}
