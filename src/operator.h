#pragma once
#include <genesis.h>

typedef struct Operator Operator;
typedef struct OperatorParameter OperatorParameter;
typedef void *updateFunc;

struct OperatorParameter
{
    const char name[6];
    u16 minSize;
    u16 value;
    u16 maxValue;
    u8 step;
    void (*onUpdate)(Operator *op);
};

struct Operator
{
    u8 opNumber;
    OperatorParameter parameters[10];
};

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

void operator_init(Operator *op, u8 opNumber);
OperatorParameter *operator_parameter(Operator *op, OpParameters parameter);
