#pragma once

#include <list>
#include <string>
#include <assert.h>
#include "utils.h"
#include "logged.h"

struct ImImg
{
    std::string  _name;
    ImTextureID  _tid = 0;
    int _w;
    int _h;
};

struct ImText
{
    typedef std::string string;
    struct Par
    {
        enum partype
        {
            par_void = 0,
            par_text,
            par_img,
        };

        Par(ImText* host, partype t) : _host(host), _type(t) {}
        virtual ~Par() {}

        ImText*         _host;
        partype         _type;


        virtual int size() const { return 0; }
    };

    struct ParText: public Par
    {
        string          _text;
        bool            _hilite = false;

        ParText(ImText* host, const char* s, bool hilite)
            : Par(host, par_text), _text(s), _hilite(hilite) {}
        
        int size() const override { return _text.size(); }
        operator const char*() const { return _text.c_str(); }
    };

    struct ParImg: public Par
    {
        ImImg   _img;

        ParImg(ImText* host, const ImImg& ii) : Par(host, par_img), _img(ii) {}
        ~ParImg()
        {
            if (_img._tid)
            {
                //LOG1("~ParImg ", _img._name);
                sg_image sgi;
                sgi.id = (uint32_t)_img._tid;
                sg_destroy_image(sgi);
                --_host->_imgCount;
            }
        }
        
    };


    typedef std::list<Par*> Pars;

    Pars        _pars;
    bool        _changed = false;
    int         _maxText = 1024*4;
    int         _size = 0;
    int         _maxImg = 3;
    int         _imgCount = 0;

    ~ImText()
    {
        clear();
    }

    void clear()
    {
        purge(_pars);
        _size = 0;
        _imgCount = 0;
    }

    void add(const char* s, bool hitlite = false)
    {
        if (s && *s)
        {
            //LOG1("IMTEXT, adding '", s << "'");
            // whole string will be a single par regardless

            Par* p = new ParText(this, s, hitlite);
            _add(p);
        }
    }

    void add(const ImImg& ii)
    {
        Par* p = new ParImg(this, ii);
        _add(p);
    }

    void render()
    {
        Pars::iterator it = _pars.begin();
        Pars::iterator ie = _pars.end();
        
        while (it != ie)
        {
            const Par* p = *it;
            ++it;

            bool last = (it == ie);

            switch (p->_type)
            {
            case Par::par_text:
                {
                    ParText* pt = (ParText*)p;
                    ImVec4 col;

                    bool lastText = last;

                    if (!lastText)
                    {
                        // are we actually the last text though?
                        lastText = true;
                        Pars::iterator i = it;
                        while (i != ie)
                        {
                            if ((*i)->_type == Par::par_text)
                            {
                                // no, more text follows
                                lastText = false;
                                break;
                            }
                            ++i;
                        }
                    }
                    
                    if (lastText)
                    {
                        if (pt->_hilite)
                        {
                            col = ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive);
                        }
                        else col = ImGui::GetStyleColorVec4(ImGuiCol_Text);
                    }
                    else
                    {
                        if (pt->_hilite)
                        {
                            col = ImGui::GetStyleColorVec4(ImGuiCol_Header);
                        }
                        else
                        {
                            col = ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled);
                        }
                    }
            
                    ImGui::PushStyleColor(ImGuiCol_Text, col);
                    ImGui::TextWrapped("%s", (const char*)*pt);
                    ImGui::PopStyleColor();
                }
                break;
            case Par::par_img:
                {
                    ParImg* pi = (ParImg*)p;
                    int w = pi->_img._w;
                    int h = pi->_img._h;

                    ImVec2 uv_min = ImVec2(0.0f, 0.0f); // Top-left
                    ImVec2 uv_max = ImVec2(1.0f, 1.0f); // Lower-right

                    // No tint
                    ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
                    
                    // 50% opaque white
                    ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f);

                    if (last)
                    {
                        border_col = ImGui::GetStyleColorVec4(ImGuiCol_Text);
                    }
                    else
                    {
                        border_col = ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled);
                    }

                    //LOG1("rendering image ", pi->_img._name << " w:" << w << " h:" << h);
                    
                    int vw = ImGui::GetContentRegionAvail().x;
                    int padw = vw - (w + 2); // border
                    padw /= 2;
                    
                    if (padw < 0)
                    {
                        double sc = ((double)vw)/(w + 2);
                        h = sc * h;
                        w = vw - 2;
                    }

                    if (padw > 0)
                    {
                       ImGui::Indent(padw);
                    }
                    
                    ImGui::Image(pi->_img._tid, ImVec2(w, h), uv_min, uv_max, tint_col, border_col);
                    if (padw > 0)
                    {
                        ImGui::Unindent(padw);
                    }
                    
                }
                break;
            }
        }
    }

protected:

    void _add(Par* p)  
    {
        // consume p
        _pars.push_back(p);
        _size += p->size();
        if (p->_type == Par::par_img) ++_imgCount;
        _trim();
        _changed = true;
    }

    void _trim()
    {
        if (_maxText)
        {
            while (_size > _maxText)
            {
                // this can also remove images, when they are
                // part of "old text".
                Pars::iterator it = _pars.begin();
                assert(it != _pars.end());
                Par* p = *it;
                _pars.pop_front();
                _size -= p->size();
                assert(_size >= 0);
                //LOG1("trimming text ", (const char*)*p);
                delete p;
            }
        }
        if (_maxImg && _imgCount > _maxImg)
        {
            auto it = _pars.begin();
            while (it != _pars.end())
            {
                if ((*it)->_type == Par::par_img)

                {
                    ParImg* pi = (ParImg*)(*it);
                    //LOG1("trimming image ", pi->_img._name);
                    it = _pars.erase(it);
                    delete pi; // adjusts imgCount
                    if (_imgCount <= _maxImg) break; // now ok.
                }
                else ++it;
            }
        }
    }
    
    
    
};
