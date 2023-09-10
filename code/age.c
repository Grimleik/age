/* ========================================================================
   Creator: Grimleik $
   ========================================================================*/
#include "age.h"

void InputUpdate(input_t *input) {
    input->activeFrame = (input->activeFrame + 1) % FRAME_COUNT;
    memcpy(input->keyStates[input->activeFrame],
           input->keyStates[WrapS32(input->activeFrame - 1, FRAME_COUNT)],
           KEY_MAX_STATES * sizeof(keyState_t));
}

void InputUpdateKey(input_t *input, u32 keyCode, b32 keyState) {
    input->keyStates[input->activeFrame][keyCode].isDown = keyState;
}

void InputUpdateAxis(input_t *input, u8 axisCode, f32 x, f32 y) {
    input->axisStates[input->activeFrame][axisCode].x = x;
    input->axisStates[input->activeFrame][axisCode].y = y;
}

b32 InputKeyPressed(input_t *input, u32 key) {
    return (input->keyStates[WrapS32(input->activeFrame, FRAME_COUNT)][key].isDown &&
            !input->keyStates[WrapS32(input->activeFrame - FRAME_PREVIOUS, FRAME_COUNT)][key].isDown);
}

b32 InputKeyDown(input_t *input, u32 key) {
    return input->keyStates[WrapS32(input->activeFrame, FRAME_COUNT)][key].isDown;
}

b32 InputKeyReleased(input_t *input, u32 key) {
    return (!input->keyStates[WrapS32(input->activeFrame, FRAME_COUNT)][key].isDown &&
            input->keyStates[WrapS32(input->activeFrame - FRAME_PREVIOUS, FRAME_COUNT)][key].isDown);
}

void *PushRenderCommand_(renderList_t *rl, RC_TYPE type, size_t sz) {

    renderCommand_t *result = (renderCommand_t *)((u8 *)rl->renderMemory + rl->renderMemoryCurrSz);
    result->type = type;
    rl->renderMemoryCurrSz += sz;
    Assert(rl->renderMemoryCurrSz < rl->renderMemoryMaxSz);
    return result;
}