#pragma once
#include <genesis.h>
#include <operator.h>

#define OPERATOR_COUNT 4
#define GLOBAL_PARAMETER_COUNT 2

typedef enum {
    PARAMETER_G_LFO_ON,
    PARAMETER_G_LFO_FREQ
} GlobalParameters;

typedef struct
{
    u16 value;
    const u16 maxValue;
    void (*onUpdate)(void);
} FmParameter;

void synth_init(void);
FmParameter *synth_globalParameter(GlobalParameters parameter);
