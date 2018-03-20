#pragma once
#include <genesis.h>
#include <operator.h>

#define OPERATOR_COUNT 4
#define FM_PARAMETER_COUNT 10

typedef enum {
    PARAMETER_G_LFO_ON,
    PARAMETER_G_LFO_FREQ,
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
    const char name[10];
    const u16 minSize;
    u16 value;
    const u16 maxValue;
    const u8 step;
    const void (*onUpdate)(void);
} FmParameter;

void synth_init(void);
void synth_playNote(void);
void synth_stopNote(void);
FmParameter *synth_fmParameter(FmParameters parameter);
Operator *synth_operator(u8 opNumber);
