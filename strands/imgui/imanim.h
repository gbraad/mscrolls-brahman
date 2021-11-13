#pragma once

#include <chrono>
#include "strutils.h"

extern ImTexLoader texLoader;

struct SGState
{
    struct 
    {
         sg_pipeline pip;
         sg_bindings bind;
    } anim;
};

extern SGState state;

struct AnimInfo
{
    typedef std::string string;

    string              _name;
    string              _anim; // json or skel
    string              _atlas;
    string              _image;  // png

    string              _play; // anim to play
    bool                _loop = false;
    int                 _delay = 0;
    int                 _track = 0;
    bool                _append = false;
    bool                _done = false;

    enum Op
    {
        op_none = 0,
        op_show,
        op_hide,
    };

    // if op defined, "play" is not an animation but a slot
    Op                  _op = op_none;

    bool validate()
    {
        // fill in missing elements
        bool r = !_name.empty() && !_anim.empty();
        if (r)
        {
            if (_atlas.empty()) _atlas = changeSuffix(_name, ".atlas");
            if (_image.empty()) _image = changeSuffix(_name, ".png");
        }
        return r;
    }


};

struct AnimState: public AnimInfo
{
    typedef std::chrono::steady_clock Clock;
    typedef std::chrono::time_point<Clock> Time;
    typedef std::list<AnimInfo*> InfoQ;

    AnimInfo*           _ai = 0;            // current
    InfoQ               _aiq;
    
    ImTex*              _tex;
    char*               _atlas;
    int                 _atlasz;
    char*               _anim;
    int                 _animz;

    bool                _begun = false;
    int                 _error = 0;
    bool                _playing = false;
    Time                _tlast;
    bool                _active = false;

    // some spine objects
    void*               _s_textureLoader = 0;
    void*               _s_renderer = 0;
    void*               _s_atlas = 0;
    void*               _s_skeldat = 0;
    void*               _s_skel = 0;
    void*               _s_statedat = 0;
    void*               _s_state = 0;

    float               _skel_x;
    float               _skel_y;
    float               _skel_w;
    float               _skel_h;

    ~AnimState()
    {
        while (pop()) ;
        setInfo(0);
    }

    bool pop()
    {
        // if present
        bool r = !_aiq.empty();
        if (r)
        {
            AnimInfo* ai = _aiq.front();
            _aiq.pop_front();
            assert(!ai->_done);
            setInfo(ai);  // replace old one
        }
        return r;
    }

    static Time now()
    {
        return Clock::now();
    }

    string name() const
    {
        return _ai ? _ai->_name : "null";
    }

    void setInfo(AnimInfo* ai)
    {
        // consume
        delete _ai;
        _ai = ai;
    }

    void queueInfo(AnimInfo* ai)
    {
        // consume
        if (!_ai || _ai->_done) setInfo(ai);
        else
        {
            // buffer
            _aiq.push_back(ai);
        }
    }
    

    void load()
    {
        // ASSUME valid
        if (!loaded())
        {
            assert(_ai);
            LOG1("anim loading ", _ai->_name);
            texLoader.getTex(_ai->_image);
            texLoader.get(_ai->_anim, false);
            texLoader.get(_ai->_atlas, false);
        }
    }

    bool loaded()
    {
        // are we loaded?
        ImTexLoader::Rec* r;

        assert(_ai);
        
        if (!_tex)
        {
            r = texLoader.find(_ai->_image);
            if (r && r->_isImage && r->_tex.valid()) _tex = &r->_tex;
        }
                    
        if (!_atlas)
        {
            r = texLoader.find(_ai->_atlas);
            if (r && r->_data && r->_sz > 0)
            {
                _atlas = r->_data;
                _atlasz = r->_sz;
            }
        }

        if (!_anim)
        {
            r = texLoader.find(_ai->_anim);
            if (r && r->_data && r->_sz > 0)
            {
                _anim = r->_data;
                _animz = r->_sz;
            }
        }

        return _tex && _atlas && _anim;
    }

    void clear()
    {
        _tex = 0;
        _atlas = 0;
        _atlasz = 0;
        _anim = 0;
        _animz = 0;
    }
};

