/* ========================================================================
   Creator: Grimleik $
   ========================================================================*/

#include "age.h"

struct triangle_t {
    v3f v[3];
};

struct mesh_t {
    std::vector<triangle_t> tris;
};

struct gameState_t {
    f32    accumulation;
    void  *memory;
    size_t memorySz;
    bool   isPaused;
    bool   shouldStepOnce;
    mesh_t *cube;
};

GAME_INIT(GameInit) {
    if (reloaded) {
        return;
    }

    gameState_t *state = (gameState_t *)platformState->memory;
    *state = {0};
    state->memory = ((u8 *)platformState->memory) + sizeof(gameState_t);
    state->memorySz = platformState->memorySize - sizeof(gameState_t);
    state->cube = new mesh_t;
    state->cube->tris = {
        {0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f},

        // EAST
        {1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f},
        {1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f},

        // NORTH
        {1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f},
        {1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f},

        // WEST
        {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f},

        // TOP
        {0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
        {0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f},

        // BOTTOM
        {1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f},
        {1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},
    };
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

    state->accumulation += platformState->deltaTime; // * 0.1f;
    float      a = (f32)platformState->renderList->windowHeight / (f32)platformState->renderList->windowWidth ;
    float      fov = 90.0f;
    float      near = 0.1f;
    float      far = 100.0f;
    mat4x4     proj = Mat4Projection(a, fov, near, far);
    mat4x4     view = Mat4Identity();
    mat4x4     model = Mat4RotationY(state->accumulation);
    rcGroup_t *group = BeginRenderGroup(platformState->renderList, proj, view, model);
    group->translation = v4f{0.0f, 0.0f, 3.0f, 0.0f};
    rcClearColor_t *cmd = PushRenderCommand(platformState->renderList, rcClearColor);
    // cmd->color = ageCOLOR_BLUE;
    cmd->color = ageCOLOR_BLACK;

    for(auto &tri : state->cube->tris) {
        auto *triCmd = PushRenderCommand(platformState->renderList, rcTriangleOutline);
        triCmd->vert_col[0] = ageCOLOR_RED;
        triCmd->vert_col[1] = ageCOLOR_GREEN;
        triCmd->vert_col[2] = ageCOLOR_BLUE;

        triCmd->vertices[0] = tri.v[0];
        triCmd->vertices[1] = tri.v[1];
        triCmd->vertices[2] = tri.v[2];
    }
#if 0
    for (int i = 0; i < 1; ++i) {
        auto *cmd3 = PushRenderCommand(platformState->renderList, rcTriangleOutline);
        cmd3->vert_col[0] = ageCOLOR_RED;
        cmd3->vert_col[1] = ageCOLOR_GREEN;
        cmd3->vert_col[2] = ageCOLOR_BLUE;

        cmd3->vertices[0] = v3f{-0.5f, -0.5f, 0.0f};
        cmd3->vertices[1] = v3f{0.0f, 0.5f, 0.0f};
        cmd3->vertices[2] = v3f{0.5f, -0.5f, 0.0f};
    }
#endif
    EndRenderGroup(group, platformState->renderList);
}

GAME_SHUTDOWN(GameShutdown) {
}