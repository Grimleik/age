#if !defined(GAME_H)
/* ========================================================================
   Creator: Grimleik $
   ========================================================================*/
#define GAME_H

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

#endif
