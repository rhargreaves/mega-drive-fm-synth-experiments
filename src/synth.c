#include <genesis.h>
#include <synth.h>

static void updateAlgorithmAndFeedback(void);
static void updateGlobalLFO(void);
static void updateStereoAndLFO(void);
static void updateFreqAndOctave(void);
static void updateNote(void);
static void updateOp1MulDt1(void);
static void updateOp1TotalLevel(void);
static void updateOp1RsAr(void);
static void updateOp1AmD1r(void);
static void updateOp1D2r(void);
static void updateOp1D1lRr(void);
static void setFrequency(u16 freq, u8 octave);
static void setAlgorithm(u8 algorithm, u8 feedback);
static void setGlobalLFO(u8 enable, u8 freq);
static void setStereoAndLFO(u8 stereo, u8 ams, u8 fms);
static void setOp1MulDt1(u8 mul, u8 dt1);
static void setOp1TotalLevel(u8 totalLevel);
static void setOp1RsAr(u8 rs, u8 ar);
static void setOp1AmD1r(u8 am, u8 d1r);
static void setOp1D2r(u8 d2r);
static void setOp1D1lRr(u8 d1l, u8 rr);

static FmParameter fmParameters[] = {
    {
        "G.LFO On ", 1, 1, 1, 1, updateGlobalLFO
    },
    {
        "G.LFO Frq", 1, 3, 7, 1, updateGlobalLFO
    },
    {
        "Note     ", 2, 1, 11, 1, updateNote
    },
    {
        "Freq Num ", 4, 653, 2047, 4, updateFreqAndOctave
    },
    {
        "Octave   ", 1, 4, 7, 1, updateFreqAndOctave
    },
    {
        "Algorithm", 1, 0, 7, 1, updateAlgorithmAndFeedback
    },
    {
        "Feedback ", 1, 0, 7, 1, updateAlgorithmAndFeedback
    },
    {
        "LFO AMS  ", 1, 0, 7, 1, updateStereoAndLFO
    },
    {
        "LFO FMS  ", 1, 0, 7, 1, updateStereoAndLFO
    },
    {
        "Stereo   ", 1, 3, 3, 1, updateStereoAndLFO
    },
    {
        "Op1 Mul  ", 2, 1, 15, 1, updateOp1MulDt1
    },
    {
        "Op1 Dt1  ", 1, 1, 7, 1, updateOp1MulDt1
    },
    {
        "Op1 TL   ", 3, 35, 127, 1, updateOp1TotalLevel
    },
    {
        "Op1 RS   ", 2, 15, 31, 1, updateOp1RsAr
    },
    {
        "Op1 AR   ", 1, 2, 3, 1, updateOp1RsAr
    },
    {
        "Op1 AM   ", 1, 1, 1, 1, updateOp1AmD1r
    },
    {
        "Op1 D1R  ", 2, 5, 31, 1, updateOp1AmD1r
    },
    {
        "Op1 D2R  ", 2, 2, 31, 1, updateOp1D2r
    },
    {
        "Op1 D1L  ", 2, 1, 15, 1, updateOp1D1lRr
    },
    {
        "Op1 RR   ", 2, 1, 15, 1, updateOp1D1lRr
    }
};

#define MAX_PARAMETERS sizeof(fmParameters) / sizeof(FmParameter)

FmParameter *s_fmParameter(FmParameters parameter)
{
    return &fmParameters[parameter];
}

u16 s_maxFmParameters(void)
{
    return MAX_PARAMETERS;
}

void s_init(void)
{
	YM2612_writeReg(0, 0x27, 0);    // Ch 3 Normal
	YM2612_writeReg(0, 0x28, 0);    // All channels off
	YM2612_writeReg(0, 0x28, 1);
	YM2612_writeReg(0, 0x28, 2);
	YM2612_writeReg(0, 0x28, 4);
	YM2612_writeReg(0, 0x28, 5);
	YM2612_writeReg(0, 0x28, 6);
    YM2612_writeReg(0, 0x90, 0);   // Proprietary
	YM2612_writeReg(0, 0x94, 0);
	YM2612_writeReg(0, 0x98, 0);
	YM2612_writeReg(0, 0x9C, 0);
}

void s_playNote(void)
{
    updateGlobalLFO();
    updateOp1MulDt1();
	YM2612_writeReg(0, 0x34, 0x0D);
	YM2612_writeReg(0, 0x38, 0x33);
	YM2612_writeReg(0, 0x3C, 0x01);
    updateOp1TotalLevel();
	YM2612_writeReg(0, 0x44, 0x2D);
	YM2612_writeReg(0, 0x48, 0x26);
	YM2612_writeReg(0, 0x4C, 0x00);
    updateOp1RsAr();
	YM2612_writeReg(0, 0x54, 0x99);
	YM2612_writeReg(0, 0x58, 0x5F);
	YM2612_writeReg(0, 0x5C, 0x99);
	updateOp1AmD1r();
	YM2612_writeReg(0, 0x64, 5);
	YM2612_writeReg(0, 0x68, 5);
	YM2612_writeReg(0, 0x6C, 7);
	updateOp1D2r();
	YM2612_writeReg(0, 0x74, 2);
	YM2612_writeReg(0, 0x78, 2);
	YM2612_writeReg(0, 0x7C, 2);
    updateOp1D1lRr();
	YM2612_writeReg(0, 0x84, 0x11);
	YM2612_writeReg(0, 0x88, 0x11);
	YM2612_writeReg(0, 0x8C, 0xA6);
    updateAlgorithmAndFeedback();
    updateStereoAndLFO();
    YM2612_writeReg(0, 0x28, 0x00); // Key off
    updateFreqAndOctave();
	YM2612_writeReg(0, 0x28, 0xF0); // Key On
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

static void setOp1MulDt1(u8 mul, u8 dt1)
{
	YM2612_writeReg(0, 0x30, mul + (dt1 << 4));
}

static void setOp1TotalLevel(u8 totalLevel)
{
	YM2612_writeReg(0, 0x40, totalLevel);
}

static void setOp1RsAr(u8 rs, u8 ar)
{
	YM2612_writeReg(0, 0x50, ar + (rs << 6));
}

static void setOp1AmD1r(u8 am, u8 d1r)
{
	YM2612_writeReg(0, 0x60, (am << 7) + d1r);
}

static void setOp1D2r(u8 d2r)
{
	YM2612_writeReg(0, 0x70, d2r);
}

static void setOp1D1lRr(u8 d1l, u8 rr)
{
	YM2612_writeReg(0, 0x80, rr + (d1l << 4));
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
        fmParameters[PARAMETER_LFO_FMS].value
    );
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
        fmParameters[PARAMETER_G_LFO_FREQ].value
    );
}

static void updateAlgorithmAndFeedback(void)
{
	setAlgorithm(
        fmParameters[PARAMETER_ALGORITHM].value,
        fmParameters[PARAMETER_FEEDBACK].value);
}

static void updateOp1MulDt1(void)
{
	setOp1MulDt1(
        fmParameters[PARAMETER_OP1_MUL].value,
        fmParameters[PARAMETER_OP1_DT1].value);
}

static void updateOp1TotalLevel(void)
{
	setOp1TotalLevel(
        fmParameters[PARAMETER_OP1_TL].value);
}

static void updateOp1RsAr(void) {
	setOp1RsAr(
        fmParameters[PARAMETER_OP1_RS].value,
        fmParameters[PARAMETER_OP1_AR].value);
}

static void updateOp1AmD1r(void)
{
	setOp1AmD1r(
        fmParameters[PARAMETER_OP1_AM].value,
        fmParameters[PARAMETER_OP1_D1R].value);
}

static void updateOp1D2r(void)
{
	setOp1D2r(fmParameters[PARAMETER_OP1_D2R].value);
}

static void updateOp1D1lRr(void)
{
	setOp1D1lRr(
        fmParameters[PARAMETER_OP1_D1L].value,
        fmParameters[PARAMETER_OP1_RR].value);
}
