#pragma once
#include <genesis.h>

#define OPERATOR_PARAMETER_COUNT 10

typedef enum {
    OP_PARAMETER_DT1,
    OP_PARAMETER_MUL,
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

struct Operator
{
    u8 opNumber;
    OperatorParameter const *parameters;
    u16 parameterValues[OPERATOR_PARAMETER_COUNT];
};

void operator_init(Operator *op, u8 opNumber, const u16 parameterValue[OPERATOR_PARAMETER_COUNT]);
u16 operator_parameterValue(Operator *op, OpParameters parameter);
void operator_parameterUpdate(Operator *op, OpParameters parameter);
void operator_setParameterValue(Operator *op, OpParameters parameter, u16 value);
void operator_update(Operator *op);
