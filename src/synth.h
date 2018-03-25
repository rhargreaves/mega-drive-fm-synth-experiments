#pragma once
#include <genesis.h>
#include <operator.h>

#define OPERATOR_COUNT 4
#define GLOBAL_PARAMETER_COUNT 2
#define FM_PARAMETER_COUNT 8

typedef enum {
    PARAMETER_G_LFO_ON,
    PARAMETER_G_LFO_FREQ
} GlobalParameters;

typedef enum {
    PARAMETER_NOTE,
    PARAMETER_FREQ,
    PARAMETER_OCTAVE,
    PARAMETER_ALGORITHM,
    PARAMETER_FEEDBACK,
    PARAMETER_LFO_AMS,
    PARAMETER_LFO_FMS,
    PARAMETER_STEREO
} FmParameters;

typedef struct
{
    u16 value;
    const u16 maxValue;
    void (*onUpdate)(void);
} FmParameter;

void synth_init(void);
void synth_playNote(void);
void synth_stopNote(void);
FmParameter *synth_fmParameter(FmParameters parameter);
FmParameter *synth_globalParameter(GlobalParameters parameter);
Operator *synth_operator(u8 opNumber);
