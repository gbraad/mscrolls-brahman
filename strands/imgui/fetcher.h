#pragma once
// set chunk_size to 0 for no chunks
#define CHUNK_SIZE 0
#define BUFFER_SIZE 2*1024*1024

#include <string>
#include "growbuf.h"
#include "logged.h"

struct Fetcher
{
    typedef std::string string;
    typedef void fetch_done(const char* name, void* ctx);
    
    char fetchBuffer[BUFFER_SIZE];
    GrowString fetchFile;
    bool loading = false;
    bool ok = true;
    fetch_done* doneFn = 0;
    void* doneCtx = 0;
    string currentName;
    
    
    static void _fetch_cb(const sfetch_response_t* r)
    {
        Fetcher* f = *(Fetcher**)r->user_data;
        f->fetch_cb(r);
    }

    void fetch_cb(const sfetch_response_t* r)
    {
        if (r->fetched)
        {
            int sz = r->fetched_size;
            int off = r->fetched_offset;
            LOG2("fetched ", r->path << " " << sz << " at " << off);
            fetchFile.append((char*)r->buffer_ptr, sz);
        }
        else if (r->failed)
        {
            LOG1("fetch ", r->path << " failed with " << r->error_code);
            switch (r->error_code)
            {
            case SFETCH_ERROR_FILE_NOT_FOUND:
                LOG1("fetch, file not found ", r->path);
                break;
            case SFETCH_ERROR_BUFFER_TOO_SMALL:
                LOG1("fetch, buffer too small ", r->path);
                break;
            case SFETCH_ERROR_INVALID_HTTP_STATUS:
                LOG1("fetch, invalid http status ", r->path);
                break;
            }
            
            ok = false;
            fetchFile.clear();
        }

        if (r->finished)
        {
            LOG3("fetch ", r->path << " finished " << fetchFile.size());
            assert(doneFn);
            loading = false;
            (*doneFn)(r->path, doneCtx);
        }
    }
    
    bool start(const string& path, fetch_done* cb, void* ctx = 0)
    {
        if (loading) return false; // wait until free
        
        sfetch_request_t fr = {};
        loading = true;
        ok = true;
        currentName = path;

        doneFn = cb;
        doneCtx = ctx;
        fr.path = currentName.c_str();
        fr.callback = _fetch_cb;
        fr.chunk_size = CHUNK_SIZE;
        fr.buffer_size = BUFFER_SIZE;
        fr.buffer_ptr = (void*)fetchBuffer;

        void* ptr = (void*)this;
        fr.user_data_ptr = (void*)&ptr;
        fr.user_data_size = sizeof(ptr);
        
        LOG1("fetching ", fr.path);
        fetchFile.clear();
        sfetch_send(&fr);
        return true;
    }

    char* yield(int& sz)
    {
        sz = fetchFile.size();
        fetchFile.add(0); // ensure zero terminate not included in size
        return fetchFile.donate();
    }

    void poll()
    {
        sfetch_dowork();
    }
};
