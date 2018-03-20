#include <genesis.h>
#include <synth.h>
#include <operator.h>

static void initOperators(void);
static void updateAlgorithmAndFeedback(void);
static void updateGlobalLFO(void);
static void updateStereoAndLFO(void);
static void updateFreqAndOctave(void);
static void updateNote(void);
static void setFrequency(u16 freq, u8 octave);
static void setAlgorithm(u8 algorithm, u8 feedback);
static void setGlobalLFO(u8 enable, u8 freq);
static void setStereoAndLFO(u8 stereo, u8 ams, u8 fms);
static void updateOperatorParameter(OpParameters opParameter, u8 opNumber);

static Operator operators[OPERATOR_COUNT];

static FmParameter fmParameters[] = {
    {"G.LFO On ", 1, 1, 1, 1, updateGlobalLFO},
    {"G.LFO Frq", 1, 3, 7, 1, updateGlobalLFO},
    {"Note     ", 2, 1, 11, 1, updateNote},
    {"Freq Num ", 4, 653, 2047, 4, updateFreqAndOctave},
    {"Octave   ", 1, 4, 7, 1, updateFreqAndOctave},
    {"Algorithm", 1, 0, 7, 1, updateAlgorithmAndFeedback},
    {"Feedback ", 1, 0, 7, 1, updateAlgorithmAndFeedback},
    {"LFO AMS  ", 1, 0, 7, 1, updateStereoAndLFO},
    {"LFO FMS  ", 1, 0, 7, 1, updateStereoAndLFO},
    {"Stereo   ", 1, 3, 3, 1, updateStereoAndLFO}};

FmParameter *s_fmParameter(FmParameters parameter)
{
    return &fmParameters[parameter];
}

Operator *s_operator(u8 opNumber)
{
    return &operators[opNumber];
}

static void initOperators(void)
{
    for (u8 i = 0; i < 4; i++)
    {
        Operator *op = &operators[i];
        operator_init(op, i);
    }
}

void s_init(void)
{
    initOperators();
    YM2612_writeReg(0, 0x27, 0); // Ch 3 Normal
    YM2612_writeReg(0, 0x28, 0); // All channels off
    YM2612_writeReg(0, 0x28, 1);
    YM2612_writeReg(0, 0x28, 2);
    YM2612_writeReg(0, 0x28, 4);
    YM2612_writeReg(0, 0x28, 5);
    YM2612_writeReg(0, 0x28, 6);
    YM2612_writeReg(0, 0x90, 0); // Proprietary
    YM2612_writeReg(0, 0x94, 0);
    YM2612_writeReg(0, 0x98, 0);
    YM2612_writeReg(0, 0x9C, 0);
}

void s_playNote(void)
{
    updateGlobalLFO();
    updateOperatorParameter(OP_PARAMETER_MUL, 0);
    YM2612_writeReg(0, 0x34, 0x0D);
    YM2612_writeReg(0, 0x38, 0x33);
    YM2612_writeReg(0, 0x3C, 0x01);
    updateOperatorParameter(OP_PARAMETER_TL, 0);
    YM2612_writeReg(0, 0x44, 0x2D);
    YM2612_writeReg(0, 0x48, 0x26);
    YM2612_writeReg(0, 0x4C, 0x00);
    updateOperatorParameter(OP_PARAMETER_RS, 0);
    YM2612_writeReg(0, 0x54, 0x99);
    YM2612_writeReg(0, 0x58, 0x5F);
    YM2612_writeReg(0, 0x5C, 0x99);
    updateOperatorParameter(OP_PARAMETER_AM, 0);
    YM2612_writeReg(0, 0x64, 5);
    YM2612_writeReg(0, 0x68, 5);
    YM2612_writeReg(0, 0x6C, 7);
    updateOperatorParameter(OP_PARAMETER_D2R, 0);
    YM2612_writeReg(0, 0x74, 2);
    YM2612_writeReg(0, 0x78, 2);
    YM2612_writeReg(0, 0x7C, 2);
    updateOperatorParameter(OP_PARAMETER_D1R, 0);
    YM2612_writeReg(0, 0x84, 0x11);
    YM2612_writeReg(0, 0x88, 0x11);
    YM2612_writeReg(0, 0x8C, 0xA6);
    updateAlgorithmAndFeedback();
    updateStereoAndLFO();
    YM2612_writeReg(0, 0x28, 0x00); // Key off
    updateFreqAndOctave();
    YM2612_writeReg(0, 0x28, 0xF0); // Key On
}

static void updateOperatorParameter(OpParameters opParameter, u8 opNumber)
{
    Operator op = operators[0];
    op.parameters[opParameter].onUpdate(&op);
}

void s_stopNote(void)
{
    YM2612_writeReg(0, 0x28, 0x00); // Key Off
}

static void setStereoAndLFO(u8 stereo, u8 ams, u8 fms)
{
    YM2612_writeReg(0, 0xB4, (stereo << 6) + (ams << 3) + fms);
}

static void setGlobalLFO(u8 enable, u8 freq)
{
    YM2612_writeReg(0, 0x22, (enable << 3) + freq);
}

static void setFrequency(u16 freq, u8 octave)
{
    YM2612_writeReg(0, 0xA4, (freq >> 8) + (octave << 3));
    YM2612_writeReg(0, 0xA0, freq);
}

static void setAlgorithm(u8 algorithm, u8 feedback)
{
    YM2612_writeReg(0, 0xB0, algorithm + (feedback << 3));
}

static void updateNote(void)
{
    const u16 notes_freq[] = {617, 653, 692, 733, 777, 823, 872, 924, 979, 1037, 1099, 1164};
    u16 note_index = fmParameters[PARAMETER_NOTE].value;
    u16 note_freq = notes_freq[note_index];
    fmParameters[PARAMETER_FREQ].value = note_freq;
    fmParameters[PARAMETER_FREQ].onUpdate();
}

static void updateStereoAndLFO(void)
{
    setStereoAndLFO(
        fmParameters[PARAMETER_STEREO].value,
        fmParameters[PARAMETER_LFO_AMS].value,
        fmParameters[PARAMETER_LFO_FMS].value);
}

static void updateFreqAndOctave(void)
{
    setFrequency(
        fmParameters[PARAMETER_FREQ].value,
        fmParameters[PARAMETER_OCTAVE].value);
}

static void updateGlobalLFO(void)
{
    setGlobalLFO(
        fmParameters[PARAMETER_G_LFO_ON].value,
        fmParameters[PARAMETER_G_LFO_FREQ].value);
}

static void updateAlgorithmAndFeedback(void)
{
    setAlgorithm(
        fmParameters[PARAMETER_ALGORITHM].value,
        fmParameters[PARAMETER_FEEDBACK].value);
}
