#include <genesis.h>
#include <stdbool.h>
#include <synth.h>
#include <channel.h>
#include <ui.h>
#include <ui_display.h>

#define SELECTION_COUNT FM_PARAMETER_COUNT + (OPERATOR_PARAMETER_COUNT * OPERATOR_COUNT)
#define INPUT_RESOLUTION 5

static bool modifyValue(u16 joyState, u16 *value);
static void checkPlayButton(u16 joyState, u16 button, Channel *channel, u16 *lastJoyState);
static void checkSelectionChangeButtons(u16 joyState, u8 selection);
static void checkValueChangeButtons(u16 joyState, u8 selection);
static void updateGlobalParameter(u16 joyState, u16 index);
static void updateOpParameter(u16 joyState, u16 index);
static void updateFmParameter(u16 joyState, u16 index);
static u8 nextChannelNumber(u8 chanNum);

static u8 currentSelection = 0;
static Channel *currentChannel;

void ui_init(void)
{
    display_init();
    currentChannel = synth_channel(0);
    display_draw(currentChannel, currentSelection);
}

void ui_checkInput(void)
{
    static u16 lastJoyStateA, lastJoyStateB;
    static u16 tick = 0;
    u16 joyState = JOY_readJoypad(JOY_1);

    checkPlayButton(joyState, BUTTON_A,
                    currentChannel, &lastJoyStateA);
    checkPlayButton(joyState, BUTTON_B,
                    synth_channel(nextChannelNumber(currentChannel->number)), &lastJoyStateB);

    if (tick % INPUT_RESOLUTION == 0)
    {
        if (joyState & BUTTON_LEFT || joyState & BUTTON_RIGHT)
        {
            checkValueChangeButtons(joyState, currentSelection);
        }
        else if (joyState & BUTTON_UP || joyState & BUTTON_DOWN)
        {
            checkSelectionChangeButtons(joyState, currentSelection);
        }
        else if (joyState & BUTTON_START)
        {
            currentChannel = synth_channel(nextChannelNumber(currentChannel->number));
            display_requestUiUpdate();
        }
    }

    display_updateUiIfRequired(currentChannel, currentSelection);
    tick++;
}

static u8 nextChannelNumber(u8 chanNum)
{
    if (++chanNum == CHANNEL_COUNT)
    {
        chanNum = 0;
    }
    return chanNum;
}

static void checkPlayButton(u16 joyState, u16 button, Channel *channel, u16 *lastJoyState)
{
    if (joyState & button)
    {
        if (!(*lastJoyState & button))
        {
            channel_playNote(channel);
        }
    }
    else
    {
        channel_stopNote(channel);
    }
    *lastJoyState = joyState;
}

static void checkSelectionChangeButtons(u16 joyState, u8 selection)
{
    if (joyState & BUTTON_DOWN)
    {
        selection += 1;
    }
    else if (joyState & BUTTON_UP)
    {
        selection -= 1;
    }
    else
    {
        return;
    }
    if (selection == (u8)-1)
    {
        selection = SELECTION_COUNT - 1;
    }
    if (selection > SELECTION_COUNT - 1)
    {
        selection = 0;
    }
    currentSelection = selection;
    display_requestUiUpdate();
}

static void checkValueChangeButtons(u16 joyState, u8 index)
{
    if (index < GLOBAL_PARAMETER_COUNT)
    {
        updateGlobalParameter(joyState, index);
        return;
    }
    index -= GLOBAL_PARAMETER_COUNT;
    if (index < FM_PARAMETER_COUNT)
    {
        updateFmParameter(joyState, index);
        return;
    }
    index -= FM_PARAMETER_COUNT;
    updateOpParameter(joyState, index);
}

static void updateGlobalParameter(u16 joyState, u16 index)
{
    u16 value = synth_globalParameterValue(index);
    if (!modifyValue(joyState, &value))
    {
        return;
    }
    synth_setGlobalParameterValue(index, value);
    display_requestUiUpdate();
}

static void updateFmParameter(u16 joyState, u16 index)
{
    u16 value = channel_parameterValue(currentChannel, index);
    if (!modifyValue(joyState, &value))
    {
        return;
    }
    channel_setParameterValue(currentChannel, index, value);
    display_requestUiUpdate();
}

static void updateOpParameter(u16 joyState, u16 index)
{
    Operator *op = channel_operator(currentChannel, index / OPERATOR_PARAMETER_COUNT);
    OpParameters opParameter = index % OPERATOR_PARAMETER_COUNT;
    u16 value = operator_parameterValue(op, opParameter);
    if (!modifyValue(joyState, &value))
    {
        return;
    }
    operator_setParameterValue(op, opParameter, value);
    display_requestUiUpdate();
}

static bool modifyValue(u16 joyState, u16 *value)
{
    if (joyState & BUTTON_RIGHT)
    {
        (*value)++;
    }
    else if (joyState & BUTTON_LEFT)
    {
        (*value)--;
    }
    else
    {
        return false;
    }
    return true;
}
