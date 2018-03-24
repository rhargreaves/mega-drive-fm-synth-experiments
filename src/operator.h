#pragma once
#include <genesis.h>

#define OPERATOR_PARAMETER_COUNT 10

typedef enum {
    OP_PARAMETER_MUL,
    OP_PARAMETER_DT1,
    OP_PARAMETER_TL,
    OP_PARAMETER_RS,
    OP_PARAMETER_AR,
    OP_PARAMETER_AM,
    OP_PARAMETER_D1R,
    OP_PARAMETER_D2R,
    OP_PARAMETER_D1L,
    OP_PARAMETER_RR
} OpParameters;

typedef struct Operator Operator;
typedef struct OperatorParameter OperatorParameter;
typedef void *updateFunc;

typedef struct OperatorDefinition
{
    u16 multiple, detune, totalLevel, rateScale,
        attackRate, ampMode, firstDecay, secondDecay,
        subLevel, releaseRate;
} OperatorDefinition;

struct Operator
{
    u8 opNumber;
    OperatorParameter *parameters;
    u16 parameterValue[OPERATOR_PARAMETER_COUNT];
};

void operator_init(Operator *op, u8 opNumber, OperatorDefinition *definition);
u16 operator_parameterValue(Operator *op, OpParameters parameter);
const char *operator_parameterName(Operator *op, OpParameters parameter);
void operator_parameterUpdate(Operator *op, OpParameters parameter);
u16 operator_parameterMinSize(Operator *op, OpParameters parameter);
u16 operator_parameterMaxValue(Operator *op, OpParameters parameter);
u8 operator_parameterStep(Operator *op, OpParameters parameter);
void operator_setParameterValue(Operator *op, OpParameters parameter, u16 value);
void operator_update(Operator *op);
