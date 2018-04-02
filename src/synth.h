#pragma once
#include <channel.h>
#include <genesis.h>

#define CHANNEL_COUNT 6
#define GLOBAL_PARAMETER_COUNT 2

typedef enum { PARAMETER_G_LFO_ON, PARAMETER_G_LFO_FREQ } GlobalParameters;

typedef struct ChannelPreset
{
    u16 channelParameters[FM_PARAMETER_COUNT];
    u16 operatorParameters[OPERATOR_COUNT][OPERATOR_PARAMETER_COUNT];
} ChannelPreset;

typedef struct Preset
{
    u16 globalParameters[GLOBAL_PARAMETER_COUNT];
    ChannelPreset channels[CHANNEL_COUNT];
} Preset;

void synth_init(void);
Channel *synth_channel(u8 number);
void synth_setGlobalParameterValue(GlobalParameters parameter, u16 value);
u16 synth_globalParameterValue(GlobalParameters parameter);
void synth_preset(const Preset *preset);
