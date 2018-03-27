#include <genesis.h>
#include <stdbool.h>
#include <synth.h>
#include <channel.h>
#include <ui.h>
#include <ui_display.h>

#define SELECTION_COUNT FM_PARAMETER_COUNT + (OPERATOR_PARAMETER_COUNT * OPERATOR_COUNT)
#define INPUT_RESOLUTION 5

static void checkPlayButton(u16 joyState, u16 button, Channel *channel, u16 *lastJoyState);
static void modifySelection(u16 joyState, u8 selection, s8 change);
static void modifyValue(u16 joyState, u8 selection, s8 change);
static void updateGlobalParameter(u16 joyState, u16 index, s8 change);
static void updateOpParameter(u16 joyState, u16 index, s8 change);
static void updateFmParameter(u16 joyState, u16 index, s8 change);
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
        if (joyState & BUTTON_LEFT)
        {
            modifyValue(joyState, currentSelection, -1);
        }
        else if (joyState & BUTTON_RIGHT)
        {
            modifyValue(joyState, currentSelection, 1);
        }
        else if (joyState & BUTTON_UP)
        {
            modifySelection(joyState, currentSelection, -1);
        }
        else if (joyState & BUTTON_DOWN)
        {
            modifySelection(joyState, currentSelection, 1);
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

static void modifySelection(u16 joyState, u8 selection, s8 change)
{
    selection += change;
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

static void modifyValue(u16 joyState, u8 index, s8 change)
{
    if (index < GLOBAL_PARAMETER_COUNT)
    {
        updateGlobalParameter(joyState, index, change);
        return;
    }
    index -= GLOBAL_PARAMETER_COUNT;
    if (index < FM_PARAMETER_COUNT)
    {
        updateFmParameter(joyState, index, change);
        return;
    }
    index -= FM_PARAMETER_COUNT;
    updateOpParameter(joyState, index, change);
}

static void updateGlobalParameter(u16 joyState, u16 index, s8 change)
{
    u16 value = synth_globalParameterValue(index);
    synth_setGlobalParameterValue(index, value + change);
    display_requestUiUpdate();
}

static void updateFmParameter(u16 joyState, u16 index, s8 change)
{
    u16 value = channel_parameterValue(currentChannel, index);
    channel_setParameterValue(currentChannel, index, value + change);
    display_requestUiUpdate();
}

static void updateOpParameter(u16 joyState, u16 index, s8 change)
{
    Operator *op = channel_operator(currentChannel, index / OPERATOR_PARAMETER_COUNT);
    OpParameters opParameter = index % OPERATOR_PARAMETER_COUNT;
    u16 value = operator_parameterValue(op, opParameter);
    operator_setParameterValue(op, opParameter, value + change);
    display_requestUiUpdate();
}
