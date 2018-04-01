#pragma once
#include <genesis.h>
#include <synth.h>

typedef struct
{
    const char name[10];
    const u16 minSize;
    FmParameter *fmParameter;
    void (*printFunc)(u16 index, u16 x, u16 y);
} FmParameterUi;

typedef struct
{
    const char name[10];
    const u16 minSize;
} OperatorParameterUi;

void display_init(void);
void display_draw(Channel *chan, u8 selection);
void display_updateUiIfRequired(Channel *chan, u8 selection);
void display_requestUiUpdate(void);
