#pragma once
#include <genesis.h>
#include <channel.h>

#define GLOBAL_PARAMETER_COUNT 2

typedef enum {
    PARAMETER_G_LFO_ON,
    PARAMETER_G_LFO_FREQ
} GlobalParameters;

void synth_init(void);
Channel *synth_channel(void);
FmParameter *synth_globalParameter(GlobalParameters parameter);
