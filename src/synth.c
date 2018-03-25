#include <genesis.h>
#include <synth.h>
#include <operator.h>
#include <channel.h>

static void updateGlobalLFO(void);
static void setGlobalLFO(u8 enable, u8 freq);

static FmParameter globalParameters[] = {
    {1, 1, updateGlobalLFO},
    {3, 7, updateGlobalLFO}};

void synth_init(void)
{
    channel_init();
    updateGlobalLFO();
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

FmParameter *synth_globalParameter(GlobalParameters parameter)
{
    return &globalParameters[parameter];
}

static void setGlobalLFO(u8 enable, u8 freq)
{
    YM2612_writeReg(0, 0x22, (enable << 3) | freq);
}

static void updateGlobalLFO(void)
{
    setGlobalLFO(
        globalParameters[PARAMETER_G_LFO_ON].value,
        globalParameters[PARAMETER_G_LFO_FREQ].value);
}
