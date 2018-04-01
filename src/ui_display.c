#include <genesis.h>
#include <synth.h>
#include <channel.h>
#include <stdbool.h>
#include <ui_display.h>

#define PAL_HEADING PAL1
#define PAL_SELECTION PAL2

#define GLOBAL_PARAMETERS_TOP_ROW 2
#define FM_PARAMETERS_VALUE_COLUMN 10
#define FM_PARAMETERS_TOP_ROW 5
#define OPERATOR_VALUE_COLUMN 10
#define OPERATOR_VALUE_WIDTH 6
#define OPERATOR_TOP_ROW 14

static void printNumber(u16 number, u16 minSize, u16 x, u16 y);
static void printNote(u16 index, u16 x, u16 y);
static void printOnOff(u16 index, u16 x, u16 y);
static void printLFOFreq(u16 index, u16 x, u16 y);
static void printLookup(u16 index, const char *text, u16 x, u16 y);
static void printGlobalParameters(u8 selection);
static void printFmHeader(Channel *chan);
static void printFmParameters(Channel *chan, u8 selection);
static void printOperators(Channel *chan, u8 selection);
static void printOperator(Operator *op, u8 selection);
static void printOperatorHeader(Operator *op);
static void printStereo(u16 index, u16 x, u16 y);
static void printAlgorithm(u16 index, u16 x, u16 y);
static void printAms(u16 index, u16 x, u16 y);
static void printFms(u16 index, u16 x, u16 y);
static void printParameter(u16 index, u16 heading_x, u16 value_x, u16 y, u8 selection);

static FmParameterUi globalParameterUis[] = {
    {"Globl LFO", 1, NULL, printOnOff},
    {"Freq", 1, NULL, printLFOFreq}};

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
    {"Detune", 1},
    {"Multiple", 2},
    {"Total Lvl", 3},
    {"Rate Scl", 1},
    {"Atck Rate", 2},
    {"Ampl Mode", 1},
    {"1st Decay", 2},
    {"2nd Decay", 2},
    {"Sub Level", 2},
    {"Rel Rate", 2},
    {"Octave", 1},
    {"Freq Num", 4}};

static bool drawUi = false;

void display_init(void)
{
    VDP_setPaletteColor((PAL1 * 16) + 15, 0x0C55);
    VDP_setPaletteColor((PAL2 * 16) + 15, 0x00DE);
    VDP_setTextPalette(PAL2);
    VDP_drawText("Yamaha 2612 Test", 11, 0);
}

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
    u16 row = GLOBAL_PARAMETERS_TOP_ROW;

    printParameter(0, 0, 10, row, selection);
    printParameter(1, 20, 25, row, selection);
}

static void printParameter(u16 index, u16 heading_x, u16 value_x, u16 y, u8 selection)
{
    FmParameterUi *p = &globalParameterUis[index];
    u16 value = synth_globalParameterValue(index);
    VDP_setTextPalette(PAL_HEADING);
    VDP_drawText(p->name, heading_x, y);
    VDP_setTextPalette(selection == index ? PAL_SELECTION : PAL0);
    if (p->printFunc != NULL)
    {
        p->printFunc(value, value_x, y);
    }
    else
    {
        printNumber(value,
                    p->minSize,
                    value_x,
                    y);
    }
}

static void printFmParameters(Channel *chan, u8 selection)
{
    printFmHeader(chan);
    for (u16 index = 0; index < FM_PARAMETER_COUNT; index++)
    {
        u16 row = index + FM_PARAMETERS_TOP_ROW;
        FmParameterUi *p = &fmParameterUis[index];
        VDP_setTextPalette(PAL_HEADING);
        VDP_drawText(p->name, 0, row);
        VDP_setTextPalette(selection == index + GLOBAL_PARAMETER_COUNT ? PAL_SELECTION : PAL0);
        u16 value = channel_parameterValue(chan, index);
        if (p->printFunc != NULL)
        {
            p->printFunc(value, 10, row);
        }
        else
        {
            printNumber(value,
                        p->minSize,
                        FM_PARAMETERS_VALUE_COLUMN,
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

static void printFmHeader(Channel *chan)
{
    VDP_setTextPalette(PAL_HEADING);
    char opHeader[4];
    sprintf(opHeader, "Ch%u", chan->number + 1);
    VDP_drawText(opHeader, FM_PARAMETERS_VALUE_COLUMN, FM_PARAMETERS_TOP_ROW - 1);
    VDP_setTextPalette(PAL0);
}

static void printOperatorHeader(Operator *op)
{
    VDP_setTextPalette(PAL_HEADING);
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
        if (op->chanNumber != 2 && (index == OP_PARAMETER_CH3_FREQ || index == OP_PARAMETER_CH3_OCTAVE))
        {
            VDP_clearText(0, row, 40);
            continue;
        }

        if (op->opNumber == 0)
        {
            VDP_setTextPalette(PAL_HEADING);
            VDP_drawText(opParameterUis[index].name, 0, row);
            VDP_setTextPalette(PAL0);
        }
        if (selection - FM_PARAMETER_COUNT == GLOBAL_PARAMETER_COUNT + index + op->opNumber * OPERATOR_PARAMETER_COUNT)
        {
            VDP_setTextPalette(PAL_SELECTION);
        }
        printNumber(operator_parameterValue(op, index),
                    opParameterUis[index].minSize,
                    OPERATOR_VALUE_WIDTH * op->opNumber + OPERATOR_VALUE_COLUMN,
                    row);
        VDP_setTextPalette(PAL0);
    }
}

static void printLFOFreq(u16 index, u16 x, u16 y)
{
    const char TEXT[][8] = {"3.98Hz", "5.56Hz", "6.02Hz", "6.37Hz", "6.88Hz", "9.63Hz", "48.1Hz", "72.2Hz"};
    printLookup(index, TEXT[index], x, y);
}

static void printOnOff(u16 index, u16 x, u16 y)
{
    const char TEXT[][4] = {"Off", "On"};
    printLookup(index, TEXT[index], x, y);
}

static void printNote(u16 index, u16 x, u16 y)
{
    const char TEXT[][3] = {"B", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#"};
    printLookup(index, TEXT[index], x, y);
}

static void printStereo(u16 index, u16 x, u16 y)
{
    const char TEXT[][4] = {"Off", "R", "L", "LR"};
    printLookup(index, TEXT[index], x, y);
}

static void printAlgorithm(u16 index, u16 x, u16 y)
{
    const char TEXT[][8] = {"1-2-3-4", "1/2-3-4", "1/23-4", "12/3-4", "1/3-2/4", "1-2/3/4", "12/3/4", "1/2/3/4"};
    printLookup(index, TEXT[index], x, y);
}

static void printAms(u16 index, u16 x, u16 y)
{
    const char TEXT[][7] = {"0", "1.4dB", "5.9dB", "11.8dB"};
    printLookup(index, TEXT[index], x, y);
}

static void printFms(u16 index, u16 x, u16 y)
{
    const char TEXT[][4] = {"0", "3.4", "6.7", "10", "14", "20", "40", "80"};
    printLookup(index, TEXT[index], x, y);
}

static void printLookup(u16 index, const char *text, u16 x, u16 y)
{
    char buffer[25];
    sprintf(buffer, "%s (%u)    ", text, index);
    VDP_drawText(buffer, x, y);
}

static void printNumber(u16 number, u16 minSize, u16 x, u16 y)
{
    char str[5];
    uintToStr(number, str, minSize);
    VDP_drawText(str, x, y);
}
