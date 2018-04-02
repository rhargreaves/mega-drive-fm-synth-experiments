#include <channel.h>
#include <genesis.h>
#include <operator.h>
#include <synth.h>

static void updateGlobalLFO(Channel *chan);
static void setGlobalLFO(u8 enable, u8 freq);

static Channel channels[6];

static FmParameter globalParameters[] = {{1, 1, updateGlobalLFO}, {3, 7, updateGlobalLFO}};

static void loadChannelPreset(ChannelPreset *chanPreset, Channel *chan);
static void loadOperatorPreset(Operator *op, u16 operatorParameters[OPERATOR_PARAMETER_COUNT]);

void synth_init(void)
{
    for (u8 i = 0; i < CHANNEL_COUNT; i++)
    {
        channel_init(&channels[i], i);
    }
    updateGlobalLFO(NULL);
    YM2612_writeReg(0, 0x27, 1 << 6); // Ch 3 Special Mode
    YM2612_writeReg(0, 0x28, 0);      // All channels off
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

Channel *synth_channel(u8 number) { return &channels[number]; }

FmParameter *synth_globalParameter(GlobalParameters parameter)
{
    return &globalParameters[parameter];
}

void synth_setGlobalParameterValue(GlobalParameters parameter, u16 value)
{
    FmParameter *fmParameter = &globalParameters[parameter];
    if (value == (u16)-1)
    {
        value = fmParameter->maxValue;
    }
    if (value > fmParameter->maxValue)
    {
        value = 0;
    }
    fmParameter->value = value;
    fmParameter->onUpdate(NULL);
}

u16 synth_globalParameterValue(GlobalParameters parameter)
{
    return globalParameters[parameter].value;
}

void synth_preset(const Preset *preset)
{
    for (u16 p = 0; p < GLOBAL_PARAMETER_COUNT; p++)
    {
        synth_setGlobalParameterValue(p, preset->globalParameters[p]);
    }
    for (u16 c = 0; c < CHANNEL_COUNT; c++)
    {
        ChannelPreset *chanPreset = &preset->channels[c];
        Channel *chan = &channels[c];
        loadChannelPreset(chanPreset, chan);
    }
}

static void loadChannelPreset(ChannelPreset *chanPreset, Channel *chan)
{
    for (u16 p = 0; p < FM_PARAMETER_COUNT; p++)
    {
        channel_setParameterValue(chan, p, chanPreset->channelParameters[p]);
    }
    for (u16 o = 0; o < OPERATOR_COUNT; o++)
    {
        Operator *op = channel_operator(chan, o);
        loadOperatorPreset(op, chanPreset->operatorParameters[o]);
    }
}

static void loadOperatorPreset(Operator *op, u16 operatorParameters[OPERATOR_PARAMETER_COUNT])
{
    for (u16 p = 0; p < OPERATOR_PARAMETER_COUNT; p++)
    {
        operator_setParameterValue(op, p, operatorParameters[p]);
    }
}

static void setGlobalLFO(u8 enable, u8 freq) { YM2612_writeReg(0, 0x22, (enable << 3) | freq); }

static void updateGlobalLFO(Channel *chan)
{
    setGlobalLFO(globalParameters[PARAMETER_G_LFO_ON].value,
                 globalParameters[PARAMETER_G_LFO_FREQ].value);
}
