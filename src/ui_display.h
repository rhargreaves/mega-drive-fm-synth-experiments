#pragma once
#include <genesis.h>
#include <synth.h>

typedef struct
{
    const char name[10];
    const u16 minSize;
    FmParameter *fmParameter;
    void (*printFunc)(u16 index, u16 row);
} FmParameterUi;

typedef struct
{
    const char name[10];
    const u16 minSize;
} OperatorParameterUi;

void display_init(Channel *chan);
void display_draw(u8 selection);
void display_updateUiIfRequired(u8 selection);
void display_requestUiUpdate(void);
