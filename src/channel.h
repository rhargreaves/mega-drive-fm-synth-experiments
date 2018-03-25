#pragma once
#include <synth.h>

#define OPERATOR_COUNT 4
#define FM_PARAMETER_COUNT 8

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

void channel_init(void);
FmParameter *channel_fmParameter(FmParameters parameter);
void channel_update(void);
Operator *channel_operator(u8 opNumber);
void channel_playNote(void);
void channel_stopNote(void);
