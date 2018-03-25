#include <genesis.h>
#include <synth.h>
#include <operator.h>
#include <channel.h>

static void updateGlobalLFO(Channel *chan);
static void setGlobalLFO(u8 enable, u8 freq);

static Channel channel;

static FmParameter globalParameters[] = {
    {1, 1, updateGlobalLFO},
    {3, 7, updateGlobalLFO}};

void synth_init(void)
{
    channel_init(&channel, 0);
    updateGlobalLFO(&channel);
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

Channel *synth_channel(void)
{
    return &channel;
}

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

static void setGlobalLFO(u8 enable, u8 freq)
{
    YM2612_writeReg(0, 0x22, (enable << 3) | freq);
}

static void updateGlobalLFO(Channel *chan)
{
    setGlobalLFO(
        globalParameters[PARAMETER_G_LFO_ON].value,
        globalParameters[PARAMETER_G_LFO_FREQ].value);
}
