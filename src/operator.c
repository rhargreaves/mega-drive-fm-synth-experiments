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

void operator_init(Operator *op, u8 opNumber)
{
    op->opNumber = opNumber;
    OperatorParameter paras[OPERATOR_PARAMETER_COUNT] = {
        {"Multiple", 2, 0, 15, 1, updateMulDt1},
        {"Detune", 1, 0, 7, 1, updateMulDt1},
        {"Total Lvl", 3, 0, 127, 1, updateTotalLevel},
        {"Rate Scale", 1, 0, 2, 1, updateRsAr},
        {"Atck Rate", 2, 0, 31, 1, updateRsAr},
        {"Amp Mode?", 1, 0, 1, 1, updateAmD1r},
        {"1st Decay", 2, 0, 31, 1, updateAmD1r},
        {"2nd Decay", 2, 0, 31, 1, updateD2r},
        {"Sub Level", 2, 0, 15, 1, updateD1lRr},
        {"Rel Rate", 2, 0, 15, 1, updateD1lRr}};
    memcpy(&op->parameters, &paras, sizeof paras);
    switch (opNumber)
    {
    case 0:
        op->parameters[OP_PARAMETER_MUL].value = 1;
        op->parameters[OP_PARAMETER_DT1].value = 1;
        op->parameters[OP_PARAMETER_TL].value = 35;
        op->parameters[OP_PARAMETER_RS].value = 15;
        op->parameters[OP_PARAMETER_AR].value = 2;
        op->parameters[OP_PARAMETER_AM].value = 1;
        op->parameters[OP_PARAMETER_D1R].value = 5;
        op->parameters[OP_PARAMETER_D2R].value = 2;
        op->parameters[OP_PARAMETER_D1L].value = 1;
        op->parameters[OP_PARAMETER_RR].value = 1;
        break;
    case 1:
        op->parameters[OP_PARAMETER_MUL].value = 13;
        op->parameters[OP_PARAMETER_DT1].value = 0;
        op->parameters[OP_PARAMETER_TL].value = 45;
        op->parameters[OP_PARAMETER_RS].value = 2;
        op->parameters[OP_PARAMETER_AR].value = 25;
        op->parameters[OP_PARAMETER_AM].value = 0;
        op->parameters[OP_PARAMETER_D1R].value = 36;
        op->parameters[OP_PARAMETER_D2R].value = 2;
        op->parameters[OP_PARAMETER_D1L].value = 1;
        op->parameters[OP_PARAMETER_RR].value = 1;
        break;
    case 2:
        op->parameters[OP_PARAMETER_MUL].value = 3;
        op->parameters[OP_PARAMETER_DT1].value = 3;
        op->parameters[OP_PARAMETER_TL].value = 38;
        op->parameters[OP_PARAMETER_RS].value = 1;
        op->parameters[OP_PARAMETER_AR].value = 31;
        op->parameters[OP_PARAMETER_AM].value = 0;
        op->parameters[OP_PARAMETER_D1R].value = 5;
        op->parameters[OP_PARAMETER_D2R].value = 2;
        op->parameters[OP_PARAMETER_D1L].value = 1;
        op->parameters[OP_PARAMETER_RR].value = 1;
        break;
    case 3:
        op->parameters[OP_PARAMETER_MUL].value = 1;
        op->parameters[OP_PARAMETER_DT1].value = 0;
        op->parameters[OP_PARAMETER_TL].value = 0;
        op->parameters[OP_PARAMETER_RS].value = 2;
        op->parameters[OP_PARAMETER_AR].value = 25;
        op->parameters[OP_PARAMETER_AM].value = 0;
        op->parameters[OP_PARAMETER_D1R].value = 7;
        op->parameters[OP_PARAMETER_D2R].value = 2;
        op->parameters[OP_PARAMETER_D1L].value = 10;
        op->parameters[OP_PARAMETER_RR].value = 6;
        break;
    }
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

OperatorParameter *operator_parameter(Operator *op, OpParameters parameter)
{
    return &op->parameters[parameter];
}
