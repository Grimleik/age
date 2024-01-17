/* ========================================================================
   Creator: Grimleik $
   ========================================================================*/
#include "age.h"

void InputUpdate(input_t *input) {
    input->activeFrame = (input->activeFrame + 1) % FRAME_COUNT;
    memcpy(input->keyStates[input->activeFrame],
           input->keyStates[Wrap(input->activeFrame - 1, FRAME_COUNT)],
           KEY_MAX_STATES * sizeof(input_t::keyState_t));
}

void InputUpdateKey(input_t *input, u32 keyCode, b32 keyState) {
    input->keyStates[input->activeFrame][keyCode].isDown = keyState;
}

void InputUpdateAxis(input_t *input, u8 axisCode, f32 x, f32 y) {
    input->axisStates[input->activeFrame][axisCode].x = x;
    input->axisStates[input->activeFrame][axisCode].y = y;
}

b32 InputKeyPressed(input_t *input, u32 key) {
    return (input->keyStates[Wrap(input->activeFrame, FRAME_COUNT)][key].isDown &&
            !input->keyStates[Wrap(input->activeFrame - FRAME_PREVIOUS, FRAME_COUNT)][key].isDown);
}

b32 InputKeyDown(input_t *input, u32 key) {
    return input->keyStates[Wrap(input->activeFrame, FRAME_COUNT)][key].isDown;
}

b32 InputKeyReleased(input_t *input, u32 key) {
    return (!input->keyStates[Wrap(input->activeFrame, FRAME_COUNT)][key].isDown &&
            input->keyStates[Wrap(input->activeFrame - FRAME_PREVIOUS, FRAME_COUNT)][key].isDown);
}

void *PushRenderCommand_(renderList_t *rl, RC_TYPE type, size_t sz) {

    renderCommand_t *result = (renderCommand_t *)((u8 *)rl->renderMemory + rl->renderMemoryCurrSz);
    *result = {};
    result->type = type;
    rl->renderMemoryCurrSz += sz;
    rl->commandCount++;
    Assert(rl->renderMemoryCurrSz < rl->renderMemoryMaxSz);
    return result;
}

rcGroup_t *BeginRenderGroup(renderList_t *rl, mat4x4 proj, mat4x4 view, mat4x4 model) {
    rcGroup_t *group = PushRenderCommand(rl, rcGroup);
    group->commandCount = rl->commandCount;
    group->projection = proj;
    group->view = view;
    group->model = model;
    group->translation = v4f{0.0f};
    rl->dbgOpenGroups++;
    return group;
}

void EndRenderGroup(rcGroup_t *group, renderList_t *rl) {
    group->commandCount = rl->commandCount - group->commandCount;
    rl->dbgOpenGroups--;
}

MemoryStack::MemoryStack() : currentSize(0), totalSize(0) {
}

MemoryStack::~MemoryStack() {
}

void MemoryStack::Initialize(void *mem, MemoryMarker size) {
    memory = mem;
    totalSize = size;
}

void MemoryStack::ResetToMarker(MemoryMarker marker) {
    currentSize = marker;
}

void MemoryStack::ZeroOutToMarker(MemoryMarker marker) {
    memset((u8 *)memory + currentSize, 0, marker - currentSize);
}

void *MemoryStack::Allocate(MemoryMarker sizeInBytes) {
    Assert(currentSize + sizeInBytes <= totalSize);
    void *result = (u8 *)memory + currentSize;
    currentSize += sizeInBytes;
    return result;
}
