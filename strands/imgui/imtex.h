#pragma once

#include <string>
#include <assert.h>
#include <list>
#include "stb_image.h"
#include "strutils.h"
#include "imwebp.h"
#include "logged.h"

#define TAG_IMTEX "ImTex, "

struct ImTex
{
    typedef std::string string;
    
    string  _name;
    ImTextureID  _tid = 0;
    int _w;
    int _h;
    int _chans;

    bool operator<(const ImTex& t) const { return _name < t._name; }

    bool create(const char* name, const unsigned char* data, int sz)
    {
        assert(!_tid);
        
        _name = name;
        const int desired_channels = 4;

        stbi_uc* pixels = 0;

        string suf = toLower(suffixOf(name));

        bool webp = (suf == ".webp");

        if (webp)
        {
            LOG3("decoding webp ", name);
            pixels = decodeWebp(data,
                                sz,
                                &_w, &_h,
                                &_chans,
                                desired_channels);

            if (!pixels)
            {
                LOG1("webp decode FAILED, ", name);
            }
        }
        else
        {
            pixels = stbi_load_from_memory(data,
                                           sz,
                                           &_w, &_h,
                                           &_chans,
                                           desired_channels);
        }

        if (pixels)
        {
            //LOG1("Loaded image got pixels ", name);
            sg_image_desc sgid = {};
            sgid.width = _w;
            sgid.height = _h;
            sgid.pixel_format = SG_PIXELFORMAT_RGBA8;
            sgid.min_filter = SG_FILTER_LINEAR;
            sgid.mag_filter = SG_FILTER_LINEAR;
            sgid.wrap_u = SG_WRAP_CLAMP_TO_EDGE;
            sgid.wrap_v = SG_WRAP_CLAMP_TO_EDGE;
            auto& si = sgid.data.subimage[0][0];
            si.ptr = pixels;
            si.size = (size_t)(_w * _h * 4);  // XX

            sg_image sgi = sg_make_image(&sgid);

            if (webp)
            {
                decodeWebpFree(pixels);
            }
            else
            {
                stbi_image_free(pixels);
            }
            _tid = (ImTextureID)sgi.id;
        }
        else
        {
            LOG1(TAG_IMTEX "Image cannot decode", _name);
        }
        return _tid != 0;
    }

    bool destroy()
    {
        if (_tid)
        {
            sg_image sgi;
            sgi.id = (uint32_t)_tid;
            sg_destroy_image(sgi);
            _tid = 0;
            return true;
        }
        return false;
    }

    bool valid() const { return _tid != 0; }
};

struct ImTexLoader
{
    typedef std::string string;
    
    struct Rec
    {
        string          _name;
        bool            _pending = false;
        bool            _failed = false;
        bool            _isImage = false;

        // this is set when we load and can be reset so that
        // we can handle one off operations after load
        bool            _justLoaded = false;
        ImTex           _tex;
        char*           _data;
        int             _sz;

        Rec(const string& name) : _name(name) { _init(); }

        bool purge()
        {
            bool r = _tex.destroy();
            if (_data)
            {
                delete _data;
                r = true;
            }
            _init();
            return r;
        }

        void _init()
        {
            _data = 0;
            _sz = 0;
        }
        
    };
    
    typedef std::list<Rec>    Pool;
    typedef std::list<Rec>    Requests;
    
    Fetcher&            _fetcher;
    Pool                _pool;
    Requests            _r;
    
    ImTexLoader(Fetcher& f) : _fetcher(f) {}

    ~ImTexLoader()
    {
        _purge(); 
    }

    void loaded(const char* name)
    {
        Requests::iterator it;
        bool found = false;
        for (it = _r.begin(); it != _r.end(); ++it)
        {
            if (it->_name == name)
            {
                found = true;
                break;
            }
        }

        if (found)
        {
            if (fetcher.ok)
            {
                Rec r = *it;
                
                char* fdata = fetcher.yield(r._sz);

                if (r._isImage)
                {
                    LOG1("Loaded image ", name << " data size:" << r._sz);
                    r._tex.create(name, (const unsigned char*)fdata, r._sz);
                    delete fdata;

                    // signal that we just loaded it.
                    r._justLoaded = true;
                }
                else
                {
                    r._data = fdata;
                }

                // remove from queue
                _r.erase(it);
                
                // add to pool
                _pool.push_back(r);
            }
            else
            {
                LOG1(TAG_IMTEX "failed to load ", name);

                // leave in queue, but mark failed so we dont
                // try loading it again
                it->_failed = true;
            }
        }
        else
        {
            LOG1(TAG_IMTEX "not found", name);
        }
    }


    void _load(const string& path, bool isImage)
    {
        for (auto& i : _r)
            if (i._name == path) return; // already queued (or failed)

        Rec r(path);
        r._isImage = isImage;
        _r.emplace_back(r);
    }

    Rec* find(const string& path)
    {
        for (auto& i : _pool)
            if (i._name == path) return &i;
        return 0;
    }
    
    Rec* get(const string& path, bool isImage)
    {
        /* if the texture is already in the pool, return it
         * otherwise perform a fetch and wait to be called later
         */

        Rec* r = find(path);

        if (!r)
        {
            // not here, load it if not already queued
            _load(path, isImage);
        }
        
        return r;
    }

    ImTex* getTex(const string& path)
    {
        Rec* r = get(path, true);
        if (r->_isImage) return &r->_tex;
        return 0;
    }

    void poll()
    {
        for (auto& r : _r)
        {
            if (r._pending || r._failed) continue;

            // keep issuing this start until accepted.
            if (fetcher.start(r._name, _loaded, this))
            {
                r._pending = true;
                LOG1("requesting file ", r._name);
            }

            // attempt only one.
            break;
        }
    }

    bool remove(const string& path)
    {
        // remove a texture from the pool
        bool r = false;

        Requests::iterator it = _r.begin();
        while (it != _r.end())
        {
            if (it->_name == path) it = _r.erase(it);
            else ++it;
        }
        
        for (Pool::iterator it = _pool.begin(); it != _pool.end(); ++it)
        {
            if (it->_name == path)
            {
                r = it->purge();
                _pool.erase(it);
                break;
            }
        }
        return r;
    }

private:

    static void _loaded(const char* name, void* ctx)
    {
        assert(ctx);
        ImTexLoader* tl = (ImTexLoader*)ctx;
        tl->loaded(name);
    }

    void _purge()
    {
        for (auto& i : _pool) i.purge();
        _pool.clear();
        _r.clear();
    }

};


