#pragma once

// forward
struct AnimState;

struct SVertex
{
    float x, y;
    float r, g, b, a;
    float u, v;
};

#define MAX_SVERTICES   2048

struct SSpineCtx
{
    typedef std::list<AnimState*>    States;

    // all active states
    States              _states;
    
    // hold here various helpers
    ImTexLoader*        _loader = 0;
    void*               _s_ext = 0;

    static SSpineCtx* get()
    {
        static SSpineCtx _ctx;
        return &_ctx;
    }

    bool update(AnimState& a);
    int render(AnimState& a);
    bool animOp(AnimState& a);
    
};
