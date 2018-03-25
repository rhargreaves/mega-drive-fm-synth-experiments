#include <operator.h>
#include <genesis.h>

static void updateMulDt1(Operator *op);
static void updateTotalLevel(Operator *op);
static void updateRsAr(Operator *op);
static void updateAmD1r(Operator *op);
static void updateD2r(Operator *op);
static void updateD1lRr(Operator *op);
static void setMulDt1(u8 opNum, u8 mul, u8 dt1);
static void setTotalLevel(u8 opNum, u8 totalLevel);
static void setRsAr(u8 opNum, u8 rs, u8 ar);
static void setAmD1r(u8 opNum, u8 am, u8 d1r);
static void setD2r(u8 opNum, u8 d2r);
static void setD1lRr(u8 opNum, u8 d1l, u8 rr);

struct OperatorParameter
{
    const u16 maxValue;
    void (*onUpdate)(Operator *op);
};

static const OperatorParameter parameters[OPERATOR_PARAMETER_COUNT] = {
    {15, updateMulDt1},
    {7, updateMulDt1},
    {127, updateTotalLevel},
    {3, updateRsAr},
    {31, updateRsAr},
    {1, updateAmD1r},
    {31, updateAmD1r},
    {31, updateD2r},
    {15, updateD1lRr},
    {15, updateD1lRr}};

void operator_init(Operator *op, u8 opNumber, const u16 parameterValues[OPERATOR_PARAMETER_COUNT])
{
    op->opNumber = opNumber;
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
    if (value > p->maxValue)
    {
        value = 0;
    }
    if (value == (u16)-1)
    {
        value = p->maxValue;
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
}

static void updateMulDt1(Operator *op)
{
    setMulDt1(
        op->opNumber,
        op->parameterValues[OP_PARAMETER_MUL],
        op->parameterValues[OP_PARAMETER_DT1]);
}

static void updateTotalLevel(Operator *op)
{
    setTotalLevel(
        op->opNumber,
        op->parameterValues[OP_PARAMETER_TL]);
}

static void updateRsAr(Operator *op)
{
    setRsAr(
        op->opNumber,
        op->parameterValues[OP_PARAMETER_RS],
        op->parameterValues[OP_PARAMETER_AR]);
}

static void updateAmD1r(Operator *op)
{
    setAmD1r(
        op->opNumber,
        op->parameterValues[OP_PARAMETER_AM],
        op->parameterValues[OP_PARAMETER_D1R]);
}

static void updateD2r(Operator *op)
{
    setD2r(
        op->opNumber,
        op->parameterValues[OP_PARAMETER_D2R]);
}

static void updateD1lRr(Operator *op)
{
    setD1lRr(
        op->opNumber,
        op->parameterValues[OP_PARAMETER_D1L],
        op->parameterValues[OP_PARAMETER_RR]);
}

static void setMulDt1(u8 opNum, u8 mul, u8 dt1)
{
    YM2612_writeReg(0, 0x30 + (opNum * 4), mul | (dt1 << 4));
}

static void setTotalLevel(u8 opNum, u8 totalLevel)
{
    YM2612_writeReg(0, 0x40 + (opNum * 4), totalLevel);
}

static void setRsAr(u8 opNum, u8 rs, u8 ar)
{
    YM2612_writeReg(0, 0x50 + (opNum * 4), ar | (rs << 6));
}

static void setAmD1r(u8 opNum, u8 am, u8 d1r)
{
    YM2612_writeReg(0, 0x60 + (opNum * 4), (am << 7) | d1r);
}

static void setD2r(u8 opNum, u8 d2r)
{
    YM2612_writeReg(0, 0x70 + (opNum * 4), d2r);
}

static void setD1lRr(u8 opNum, u8 d1l, u8 rr)
{
    YM2612_writeReg(0, 0x80 + (opNum * 4), rr | (d1l << 4));
}
