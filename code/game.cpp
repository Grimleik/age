/* ========================================================================
   Creator: Grimleik $
   ========================================================================*/

// TODO: Render a Triangle.
// TODO: Incorporate OpenGL.
// TODO: 3D Rendering.
// TODO: Start CubeWorld.

#include "age.h"

struct gameState_t {
    f32    accumulation;
    void  *memory;
    size_t memorySz;
    bool   isPaused;
    bool   shouldStepOnce;
};

GAME_INIT(GameInit) {
    if (reloaded) {
        return;
    }

    gameState_t *state = (gameState_t *)platformState->memory;
    *state = {0};
    state->memory = ((u8 *)platformState->memory) + sizeof(gameState_t);
    state->memorySz = platformState->memorySize - sizeof(gameState_t);
}

GAME_UPDATE(GameUpdate) {
    gameState_t *state = (gameState_t *)platformState->memory;
    if (InputKeyPressed(platformState->input, KEY_ESCAPE)) {
        platformState->isRunning = false;
    }
    if (InputKeyPressed(platformState->input, KEY_P)) {
        state->isPaused = !state->isPaused;
    }
    if (state->isPaused) {
        if (InputKeyPressed(platformState->input, KEY_N) ||
            InputKeyDown(platformState->input, KEY_M)) {
            state->shouldStepOnce = true;
        }
        if (state->shouldStepOnce) {
            state->shouldStepOnce = false;
        } else {
            return;
        }
    }

    state->accumulation += platformState->deltaTime;// * 0.1f;
    rcClearColor_t *cmd = PushRenderCommand(platformState->renderList, rcClearColor);
    // cmd->color = ageCOLOR_BLUE;
    cmd->color = ageCOLOR_BLACK;

    rcLine_t *cmd2 = PushRenderCommand(platformState->renderList, rcLine);
    cmd2->p0 = v2f{2.5f, 2.5f};
    cmd2->col0 = ageCOLOR_RED;
    cmd2->p1 = v2f{2.5f, 10.5f};
    cmd2->col1 = ageCOLOR_GREEN;

    for (int i = 0; i < 1; ++i) {
        rcTriangleOutline_t *cmd3 = PushRenderCommand(platformState->renderList, rcTriangleOutline);
        if (i == 0)
            cmd3->vert_col[0] = cmd3->vert_col[1] = cmd3->vert_col[2] = ageCOLOR_GREEN;
        else if (i == 1)
            cmd3->vert_col[0] = cmd3->vert_col[1] = cmd3->vert_col[2] = ageCOLOR_RED;
        else
            cmd3->vert_col[0] = cmd3->vert_col[1] = cmd3->vert_col[2] = ageCOLOR_BLUE;

        // f32 dtSin = (f32)sin(state->accumulation);
        cmd3->vertices[0] = v3f{5.0f + i * 2, 6.0f, 0.0f};
        cmd3->vertices[1] = v3f{10.0f + i * 2, 5.0f + 5.0f * (f32)sin(1.0f), 0.0f};
        cmd3->vertices[2] = v3f{15.0f + i * 2, 4.0f, 0.0f};
    }
}

GAME_SHUTDOWN(GameShutdown) {
}