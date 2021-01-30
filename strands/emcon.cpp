#ifdef __EMSCRIPTEN__

#include <stdio.h>
#include <string>
#include <emscripten/fetch.h>
#include <emscripten.h>

struct ConReader
{
    bool _ready;
    std::string _data;
    
    static void _read_success(emscripten_fetch_t *fetch)
    {
        ConReader* cr = (ConReader*)fetch->userData;
        if (!cr)
        {
            printf("ConReader: Bad host!\n");
            emscripten_fetch_close(fetch);
        }
        else
        {
            cr->read_success(fetch);
        }
    }

    void read_success(emscripten_fetch_t *fetch)
    {
        // can be zero bytes
        _data = std::string(fetch->data, fetch->numBytes);
        emscripten_fetch_close(fetch);
        _ready = true;
    }

    static void _read_fail(emscripten_fetch_t *fetch)
    {
        printf("ConReader: fetch fail!\n");
        emscripten_fetch_close(fetch);
    }
    
    static void read_async(void* host)
    {
        emscripten_fetch_attr_t attr;
        emscripten_fetch_attr_init(&attr);
        strcpy(attr.requestMethod, "GET");
        attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
        attr.onsuccess = _read_success;
        attr.onerror = _read_fail;
        emscripten_fetch_t* f = emscripten_fetch(&attr, "___terminal::read");
        f->userData = host;
    }

    bool read_sync()
    {
        _ready = false;
        read_async(this);        
        while (!_ready) emscripten_sleep(100);
        return true;
    }
};


std::string em_getline()
{
    ConReader cr;
    cr.read_sync();
    //printf("em_getline (%d) '%s'\n", (int)cr._data.size(), cr._data.c_str());
    return cr._data;
}

#endif // __EMSCRIPTEN__
