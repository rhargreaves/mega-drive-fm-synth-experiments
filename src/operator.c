#include <operator.h>
#include <genesis.h>

#define OP_MAX_PARAMETERS 10

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

void operator_init(Operator *op, u8 opNumber)
{
    OperatorParameter paras[] = {
        {"Mul   ", 2, 1, 15, 1, updateMulDt1},
        {"Dt1   ", 1, 1, 7, 1, updateMulDt1},
        {"TL    ", 3, 35, 127, 1, updateTotalLevel},
        {"RS    ", 2, 15, 31, 1, updateRsAr},
        {"AR    ", 1, 2, 3, 1, updateRsAr},
        {"AM    ", 1, 1, 1, 1, updateAmD1r},
        {"D1R   ", 2, 5, 31, 1, updateAmD1r},
        {"D2R   ", 2, 2, 31, 1, updateD2r},
        {"D1L   ", 2, 1, 15, 1, updateD1lRr},
        {"RR    ", 2, 1, 15, 1, updateD1lRr}};
    memcpy(&op->parameters, &paras, sizeof paras);
}

static void updateMulDt1(Operator *op)
{
    setMulDt1(
        op->opNumber,
        op->parameters[OP_PARAMETER_MUL].value,
        op->parameters[OP_PARAMETER_DT1].value);
}

static void updateTotalLevel(Operator *op)
{
    setTotalLevel(
        op->opNumber,
        op->parameters[OP_PARAMETER_TL].value);
}

static void updateRsAr(Operator *op)
{
    setRsAr(
        op->opNumber,
        op->parameters[OP_PARAMETER_RS].value,
        op->parameters[OP_PARAMETER_AR].value);
}

static void updateAmD1r(Operator *op)
{
    setAmD1r(
        op->opNumber,
        op->parameters[OP_PARAMETER_AM].value,
        op->parameters[OP_PARAMETER_D1R].value);
}

static void updateD2r(Operator *op)
{
    setD2r(
        op->opNumber,
        op->parameters[OP_PARAMETER_D2R].value);
}

static void updateD1lRr(Operator *op)
{
    setD1lRr(
        op->opNumber,
        op->parameters[OP_PARAMETER_D1L].value,
        op->parameters[OP_PARAMETER_RR].value);
}

static void setMulDt1(u8 opNum, u8 mul, u8 dt1)
{
    YM2612_writeReg(0, 0x30 + (opNum * 4), mul + (dt1 << 4));
}

static void setTotalLevel(u8 opNum, u8 totalLevel)
{
    YM2612_writeReg(0, 0x40 + (opNum * 4), totalLevel);
}

static void setRsAr(u8 opNum, u8 rs, u8 ar)
{
    YM2612_writeReg(0, 0x50 + (opNum * 4), ar + (rs << 6));
}

static void setAmD1r(u8 opNum, u8 am, u8 d1r)
{
    YM2612_writeReg(0, 0x60 + (opNum * 4), (am << 7) + d1r);
}

static void setD2r(u8 opNum, u8 d2r)
{
    YM2612_writeReg(0, 0x70 + (opNum * 4), d2r);
}

static void setD1lRr(u8 opNum, u8 d1l, u8 rr)
{
    YM2612_writeReg(0, 0x80 + (opNum * 4), rr + (d1l << 4));
}

u16 operator_parameterCount(void)
{
    return OP_MAX_PARAMETERS;
}
