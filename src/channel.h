#pragma once
#include <genesis.h>
#include <operator.h>

#define OPERATOR_COUNT 4
#define FM_PARAMETER_COUNT 8

typedef struct Channel Channel;
typedef struct FmParameter FmParameter;

struct FmParameter
{
    u16 value;
    const u16 maxValue;
    void (*onUpdate)(Channel *chan);
};

struct Channel
{
    Operator operators[OPERATOR_COUNT];
    FmParameter fmParameters[FM_PARAMETER_COUNT];
};

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

void channel_init(Channel *chan);
FmParameter *channel_fmParameter(Channel *chan, FmParameters parameter);
void channel_update(Channel *chan);
Operator *channel_operator(Channel *chan, u8 opNumber);
void channel_playNote(Channel *chan);
void channel_stopNote(Channel *chan);
