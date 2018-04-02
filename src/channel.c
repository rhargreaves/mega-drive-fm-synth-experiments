#include <channel.h>
#include <megadrive.h>

static void updateAlgorithmAndFeedback(Channel *chan);
static void updateStereoAndLFO(Channel *chan);
static void updateFreqAndOctave(Channel *chan);
static void updateNote(Channel *chan);
static void setFrequency(Channel *chan, u16 freq, u8 octave);
static void setAlgorithm(Channel *chan, u8 algorithm, u8 feedback);
static void setStereoAndLFO(Channel *chan, u8 stereo, u8 ams, u8 fms);
static void keyOn(Channel *chan);
static void keyOff(Channel *chan);
static u8 keyRegValue(Channel *chan);

static const u16 defaultOperatorValues[OPERATOR_COUNT][OPERATOR_PARAMETER_COUNT] = {0};

FmParameter *channel_fmParameter(Channel *chan, FmParameters parameter)
{
    return &chan->fmParameters[parameter];
}

Operator *channel_operator(Channel *chan, u8 opNumber) { return &chan->operators[opNumber]; }

void channel_init(Channel *chan, u8 number)
{
    chan->number = number;
    FmParameter fmParas[] = {{1, 11, updateNote},
                             {653, 2047, updateFreqAndOctave},
                             {4, 7, updateFreqAndOctave},
                             {0, 7, updateAlgorithmAndFeedback},
                             {0, 7, updateAlgorithmAndFeedback},
                             {0, 3, updateStereoAndLFO},
                             {0, 7, updateStereoAndLFO},
                             {3, 3, updateStereoAndLFO}};
    memcpy(&chan->fmParameters[0], &fmParas, sizeof(FmParameter) * FM_PARAMETER_COUNT);
    for (u8 i = 0; i < OPERATOR_COUNT; i++)
    {
        operator_init(&chan->operators[i], i, chan->number, &defaultOperatorValues[i][0]);
    }
}

void channel_update(Channel *chan)
{
    for (int i = 0; i < OPERATOR_COUNT; i++)
    {
        operator_update(&chan->operators[i]);
    }
    updateAlgorithmAndFeedback(chan);
    updateStereoAndLFO(chan);
    updateFreqAndOctave(chan);
}

void channel_playNote(Channel *chan)
{
    keyOff(chan);
    channel_update(chan);
    keyOn(chan);
}

void channel_stopNote(Channel *chan) { keyOff(chan); }

void channel_setParameterValue(Channel *chan, FmParameters parameter, u16 value)
{
    FmParameter *fmParameter = &chan->fmParameters[parameter];
    if (value == (u16)-1)
    {
        value = fmParameter->maxValue;
    }
    if (value > fmParameter->maxValue)
    {
        value = 0;
    }
    fmParameter->value = value;
    fmParameter->onUpdate(chan);
}

u16 channel_parameterValue(Channel *chan, FmParameters parameter)
{
    return chan->fmParameters[parameter].value;
}

static void keyOn(Channel *chan)
{
    u8 chanRegValue = keyRegValue(chan);
    YM2612_writeReg(0, 0x28, 0xF0 | chanRegValue);
}
static void keyOff(Channel *chan)
{
    u8 chanRegValue = keyRegValue(chan);
    YM2612_writeReg(0, 0x28, 0x00 | chanRegValue);
}

static u8 keyRegValue(Channel *chan)
{
    u8 channelReg = chan->number;
    return (chan->number > 2) ? channelReg + 1 : channelReg;
}

static void setStereoAndLFO(Channel *chan, u8 stereo, u8 ams, u8 fms)
{
    megadrive_writeToYm2612(chan->number, 0xB4, (stereo << 6) | (ams << 4) | fms);
}

static void setFrequency(Channel *chan, u16 freq, u8 octave)
{
    megadrive_writeToYm2612(chan->number, 0xA4, (freq >> 8) | (octave << 3));
    megadrive_writeToYm2612(chan->number, 0xA0, freq);
}

static void setAlgorithm(Channel *chan, u8 algorithm, u8 feedback)
{
    megadrive_writeToYm2612(chan->number, 0xB0, algorithm | (feedback << 3));
}

static void updateNote(Channel *chan)
{
    const u16 notes_freq[] = {617, 653, 692, 733, 777, 823, 872, 924, 979, 1037, 1099, 1164};
    u16 note_index = chan->fmParameters[PARAMETER_NOTE].value;
    u16 note_freq = notes_freq[note_index];
    chan->fmParameters[PARAMETER_FREQ].value = note_freq;
    chan->fmParameters[PARAMETER_FREQ].onUpdate(chan);
}

static void updateStereoAndLFO(Channel *chan)
{
    setStereoAndLFO(chan, chan->fmParameters[PARAMETER_STEREO].value,
                    chan->fmParameters[PARAMETER_LFO_AMS].value,
                    chan->fmParameters[PARAMETER_LFO_FMS].value);
}

static void updateFreqAndOctave(Channel *chan)
{
    setFrequency(chan, chan->fmParameters[PARAMETER_FREQ].value,
                 chan->fmParameters[PARAMETER_OCTAVE].value);
}

static void updateAlgorithmAndFeedback(Channel *chan)
{
    setAlgorithm(chan, chan->fmParameters[PARAMETER_ALGORITHM].value,
                 chan->fmParameters[PARAMETER_FEEDBACK].value);
}
