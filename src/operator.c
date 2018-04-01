#include <operator.h>
#include <genesis.h>
#include <megadrive.h>

static void updateMulDt1(Operator *op);
static void updateTotalLevel(Operator *op);
static void updateRsAr(Operator *op);
static void updateAmD1r(Operator *op);
static void updateD2r(Operator *op);
static void updateD1lRr(Operator *op);
static void updateFreqAndOctave(Operator *op);
static void setMulDt1(Operator *op, u8 mul, u8 dt1);
static void setTotalLevel(Operator *op, u8 totalLevel);
static void setRsAr(Operator *op, u8 rs, u8 ar);
static void setAmD1r(Operator *op, u8 am, u8 d1r);
static void setD2r(Operator *op, u8 d2r);
static void setD1lRr(Operator *op, u8 d1l, u8 rr);
static void setFreqAndOctave(Operator *op, u8 octave, u16 freq);

struct OperatorParameter
{
    const u16 maxValue;
    void (*onUpdate)(Operator *op);
};

static const OperatorParameter parameters[OPERATOR_PARAMETER_COUNT] = {
    {7, updateMulDt1},
    {15, updateMulDt1},
    {127, updateTotalLevel},
    {3, updateRsAr},
    {31, updateRsAr},
    {1, updateAmD1r},
    {31, updateAmD1r},
    {31, updateD2r},
    {15, updateD1lRr},
    {15, updateD1lRr},
    {7, updateFreqAndOctave},
    {2047, updateFreqAndOctave}};

void operator_init(Operator *op, u8 opNumber, u8 chanNumber, const u16 parameterValues[OPERATOR_PARAMETER_COUNT])
{
    op->opNumber = opNumber;
    op->chanNumber = chanNumber;
    op->parameters = &parameters[0];
    for (int i = 0; i < OPERATOR_PARAMETER_COUNT; i++)
    {
        op->parameterValues[i] = parameterValues[i];
    }
}

u16 operator_parameterValue(Operator *op, OpParameters parameter)
{
    return op->parameterValues[parameter];
}

void operator_setParameterValue(Operator *op, OpParameters parameter, u16 value)
{
    const OperatorParameter *p = &op->parameters[parameter];
    if (value == (u16)-1)
    {
        value = p->maxValue;
    }
    if (value > p->maxValue)
    {
        value = 0;
    }
    op->parameterValues[parameter] = value;
    op->parameters[parameter].onUpdate(op);
}

void operator_update(Operator *op)
{
    updateMulDt1(op);
    updateTotalLevel(op);
    updateRsAr(op);
    updateAmD1r(op);
    updateD2r(op);
    updateD1lRr(op);
    updateFreqAndOctave(op);
}

static void updateMulDt1(Operator *op)
{
    setMulDt1(
        op,
        op->parameterValues[OP_PARAMETER_MUL],
        op->parameterValues[OP_PARAMETER_DT1]);
}

static void updateTotalLevel(Operator *op)
{
    setTotalLevel(
        op,
        op->parameterValues[OP_PARAMETER_TL]);
}

static void updateRsAr(Operator *op)
{
    setRsAr(
        op,
        op->parameterValues[OP_PARAMETER_RS],
        op->parameterValues[OP_PARAMETER_AR]);
}

static void updateAmD1r(Operator *op)
{
    setAmD1r(
        op,
        op->parameterValues[OP_PARAMETER_AM],
        op->parameterValues[OP_PARAMETER_D1R]);
}

static void updateD2r(Operator *op)
{
    setD2r(
        op,
        op->parameterValues[OP_PARAMETER_D2R]);
}

static void updateD1lRr(Operator *op)
{
    setD1lRr(
        op,
        op->parameterValues[OP_PARAMETER_D1L],
        op->parameterValues[OP_PARAMETER_RR]);
}

static void updateFreqAndOctave(Operator *op)
{
    setFreqAndOctave(
        op,
        op->parameterValues[OP_PARAMETER_CH3_OCTAVE],
        op->parameterValues[OP_PARAMETER_CH3_FREQ]);
}

static void setMulDt1(Operator *op, u8 mul, u8 dt1)
{
    megadrive_writeToYm2612(op->chanNumber, 0x30 + (op->opNumber * 4), mul | (dt1 << 4));
}

static void setTotalLevel(Operator *op, u8 totalLevel)
{
    megadrive_writeToYm2612(op->chanNumber, 0x40 + (op->opNumber * 4), totalLevel);
}

static void setRsAr(Operator *op, u8 rs, u8 ar)
{
    megadrive_writeToYm2612(op->chanNumber, 0x50 + (op->opNumber * 4), ar | (rs << 6));
}

static void setAmD1r(Operator *op, u8 am, u8 d1r)
{
    megadrive_writeToYm2612(op->chanNumber, 0x60 + (op->opNumber * 4), (am << 7) | d1r);
}

static void setD2r(Operator *op, u8 d2r)
{
    megadrive_writeToYm2612(op->chanNumber, 0x70 + (op->opNumber * 4), d2r);
}

static void setD1lRr(Operator *op, u8 d1l, u8 rr)
{
    megadrive_writeToYm2612(op->chanNumber, 0x80 + (op->opNumber * 4), rr | (d1l << 4));
}

static void setFreqAndOctave(Operator *op, u8 octave, u16 freq)
{
    u8 lower_reg, upper_reg;
    if (op->opNumber == 1)
    {
        lower_reg = 0xA8;
        upper_reg = 0xAC;
    }
    else if (op->opNumber == 2)
    {
        lower_reg = 0xA9;
        upper_reg = 0xAD;
    }
    else if (op->opNumber == 3)
    {
        lower_reg = 0xAA;
        upper_reg = 0xAE;
    }
    else
    {
        return;
    }
    megadrive_writeToYm2612(0, upper_reg, (freq >> 8) | (octave << 3));
    megadrive_writeToYm2612(0, lower_reg, freq);
}
