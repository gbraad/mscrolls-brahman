
#pragma once

#include <list>
#include <assert.h>
#include "growbuf.h"
#include "utils.h"
#include "logged.h"

struct ImText
{
    struct Par
    {
        GrowString      _text;
        bool            _hilite = false;

        Par(const char* s, bool hilite) : _hilite(hilite)
        {
            _text.append(s);
            _text.add(0);
        }

        int size() const { return _text.size(); }
        operator const char*() const { return _text.start(); }
    };

    typedef std::list<Par*> Pars;

    Pars        _pars;
    bool        _changed = false;
    int         _maxText = 1024*4;
    int         _size = 0;

    ~ImText()
    {
        clear();
    }

    void clear()
    {
        purge(_pars);
        _size = 0;
    }


    void add(const char* s, bool hitlite = false)
    {
        if (s && *s)
        {
            //LOG1("IMTEXT, adding '", s << "'");
            // whole string will be a single par regardless

            Par* p = new Par(s, hitlite);
            _add(p);
        }
    }

    void render()
    {
        Pars::iterator it = _pars.begin();
        Pars::iterator ie = _pars.end();
        
        while (it != ie)
        {
            const Par* p = *it;
            ++it;

            ImVec4 col;

            bool last = (it == ie);

            if (last)
            {
                if (p->_hilite)
                {
                    col = ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive);
                }
                else col = ImGui::GetStyleColorVec4(ImGuiCol_Text);
            }
            else
            {
                if (p->_hilite)
                {
                    col = ImGui::GetStyleColorVec4(ImGuiCol_Header);
                }
                else
                {
                    col = ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled);
                }
            }
            
            ImGui::PushStyleColor(ImGuiCol_Text, col);
            ImGui::TextWrapped("%s", (const char*)*p);
            ImGui::PopStyleColor();
        }
    }

protected:

    void _add(Par* p)  
    {
        // consume p
        _size += p->size();
        _pars.push_back(p);
        _trim();
        //printf("Text size %d\n", _size);
        _changed = true;
    }

    void _trim()
    {
        if (_maxText)
        {
            while (_size > _maxText)
            {
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
    }
    
    
    
};
