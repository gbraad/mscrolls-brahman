#pragma once

#include <list>
#include <string>
#include <assert.h>
#include "utils.h"
#include "logged.h"

#include "imgui_markdown.h"
#include "IconsFontAwesome5.h"
#include "imanim.h"

#define TAG_IMTEXT  "ImText, "

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
            par_anim,
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
    };

    struct ParImg: public Par
    {
        string   _name;
        
        ParImg(ImText* host, const string& name)
            : Par(host, par_img), _name(name)
        {
            // will initiate loading if not in pool
            texLoader.getTex(name);
        }
        
        ~ParImg()
        {
            // XX what about multiple refs to same image
            if (texLoader.remove(_name))
                --_host->_imgCount;
        }
    };

    struct ParAnim: public Par
    {
        AnimState _a;

        ParAnim(ImText* host) : Par(host, par_anim)
        {
            _a.clear();
        }

    };

    typedef std::list<Par*> Pars;

    Pars        _pars;
    bool        _changed = false;
    int         _maxText = 1024*4;
    int         _size = 0;
    int         _maxImg = 3;
    int         _imgCount = 0;

    // set when we click on a command link
    // this is picked up by yieldCmd
    string      _clickCommand;

    // markdown
    ImGui::MarkdownConfig       _mdConfig;
    ImFont* H1 = NULL;  // bold big
    ImFont* H2 = NULL;  // italic
    ImFont* H3 = NULL;  // bold

    void setBoldFont(ImFont* f)
    {
        H1 = H3 = f;
    }

    void setItalicFont(ImFont* f)
    {
        H2 = f;
    }

    ImText()
    {
        _mdConfig.linkCallback = &_linkCallback;
        _mdConfig.tooltipCallback = NULL;
        _mdConfig.imageCallback = &_imageCallback;
        _mdConfig.linkIcon = ICON_FA_LINK;
        _mdConfig.formatCallback = &_formatCallback;
        _mdConfig.userData = (void*)this;
    }

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
            //LOG1(TAG_IMTEXT "adding '", s << "'");
            // whole string will be a single par regardless

            Par* p = new ParText(this, s, hitlite);
            _add(p);
        }
    }

    void addImage(const string& name)
    {
        Par* p = new ParImg(this, name);
        _add(p);
    }


    void linkCallback(ImGui::MarkdownLinkCallbackData d)
    {
        // called when a link is clicked
        if (!d.isImage)
        {
            string url(d.link, d.linkLength);

            if (startsWith(url, "http://") || startsWith(url, "https://"))
            {
                LOG1(TAG_IMTEXT "link clicked ", url);
                
#ifdef __EMSCRIPTEN__
	/* Implementation in pre.js */
            EM_ASM({ if(window["open_url"]) window.open_url($0, $1) }, url.c_str(), url.length());
#endif
            }
            else if (!url.empty())
            {
                // treat as a command, send back to game

                LOG1(TAG_IMTEXT "command clicked ", url);
                _clickCommand = url;
            }
        }
    }

    ImGui::MarkdownImageData imageCallback(ImGui::MarkdownLinkCallbackData d)
    {
        string url(d.link, d.linkLength);
        //LOG1(TAG_IMTEXT "imagecallback ", url);

        assert(d.isImage);
        
        ImGui::MarkdownImageData imageData;

        ImTex* tex = texLoader.getTex(url);
        if (tex)
        {
            assert(tex->_tid && tex->_w > 0 && tex->_h > 0);
            
            imageData.isValid =         true;
            //imageData.useLinkCallback = false;
            imageData.user_texture_id = tex->_tid;
            imageData.size = ImVec2(tex->_w, tex->_h);

            // For image resize when available size.x > image width, add
            ImVec2 const contentSize = ImGui::GetContentRegionAvail();
            if( imageData.size.x > contentSize.x )
            {
                float const ratio = imageData.size.y/imageData.size.x;
                imageData.size.x = contentSize.x;
                imageData.size.y = contentSize.x*ratio;
            }
        }
        return imageData;
    }

    void formatCallback(const ImGui::MarkdownFormatInfo& info, bool start)
    {
        bool done = false;
     
        switch(info.type )
        {
        case ImGui::MarkdownFormatType::EMPHASIS:
            {
                const ImGui::MarkdownHeadingFormat* fmt = 0;
                done = true;
                
                if (info.level == 1 )
                {
                    // normal emphasis (italic)
                    fmt = &info.config->headingFormats[ImGui::MarkdownConfig::NUMHEADINGS - 2 ];
                }
                else
                {
                    // strong emphasis
                    fmt = &info.config->headingFormats[ImGui::MarkdownConfig::NUMHEADINGS - 1 ];
                }

                if (fmt && fmt->font)
                {
                    if (start) ImGui::PushFont(fmt->font);
                    else ImGui::PopFont();
                }
                else
                {
                    if (start)
                        ImGui::PushStyleColor( ImGuiCol_Text, ImGui::GetStyle().Colors[ ImGuiCol_TextDisabled ] );
                    else ImGui::PopStyleColor();
                }
            }
            break;
        }

        if (!done)
            ImGui::defaultMarkdownFormatCallback(info, start);
    }

    void renderMarkdown(const string& t)
    {
        //ImGui::TextWrapped("%s", t.c_str());
        
        _mdConfig.headingFormats[0] =    { H1, true };
        _mdConfig.headingFormats[1] =    { H2, true };
        _mdConfig.headingFormats[2] =    { H3, false };
        
        ImGui::Markdown(t.c_str(), t.length(), _mdConfig);
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
                    renderMarkdown(pt->_text);
                    ImGui::PopStyleColor();
                }
                break;
            case Par::par_img:
                {
                    ParImg* pi = (ParImg*)p;

                    // is it loaded yet?
                    // do not demand load, just check
                    ImTex* tex = 0;
                    ImTexLoader::Rec* r = texLoader.find(pi->_name);
                    if (r && r->_isImage && r->_tex.valid()) tex = &r->_tex;

                    if (tex)
                    {
                        int w = tex->_w;
                        int h = tex->_h;

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

                        if (padw > 0) ImGui::Indent(padw);
                        ImGui::Image(tex->_tid, ImVec2(w, h), uv_min, uv_max, tint_col, border_col);
                        if (padw > 0) ImGui::Unindent(padw);
                    }
                }
                break;
            case Par::par_anim:
                {
                    ParAnim* pa = (ParAnim*)p;
#ifdef USESPINE
                    if (pa->_a._active && pa->_a.loaded())
                    {
                        SSpineCtx* ctx = SSpineCtx::get();
                        bool changed = ctx->update(pa->_a);

                        if (changed)
                        {

                            int w = pa->_a._skel_w;
                            int h = pa->_a._skel_h;

                            int vw = ImGui::GetContentRegionAvail().x;
                            int padw = vw - (w + 2); // border
                            padw /= 2;
                    
                            if (padw < 0)
                            {
                                double sc = ((double)vw)/(w + 2);
                                h = sc * h;
                                w = vw - 2;
                            }

                            if (padw > 0) ImGui::Indent(padw);
                            
                            if (ImGui::BeginChild("anim", ImVec2(w, h), true))
                            {
                                // render?
                                ImGui::GetWindowDrawList()->AddCallback(render_anim,
                                                                        pa);
                            }
                            ImGui::EndChild();
                            if (padw > 0) ImGui::Unindent(padw);
                        }
                    }
#endif                    
                }
                break;
            }
        }
    }

#ifdef USESPINE

    ParAnim* bump(AnimInfo* ai)
    {
        // find animation and bump to active
        ParAnim* p = 0;

        // see if we have this animation already
        Pars::iterator it = _pars.begin();
        while (it != _pars.end())
        {
            Par* pi = *it;
            if (pi->_type == Par::par_anim)
            {
                ParAnim* pa = (ParAnim*)pi;
                if (!p && pa->_a.name() == ai->_name)
                {
                    p = pa;

                    // remove from list and continue to deactivate
                    // all others, then add to end later.
                    it = _pars.erase(it);
                    continue;
                }
                else
                {
                    // deactivate all others
                    pa->_a._active = false; 
                }
            }
            ++it;
        }

        if (p)
        {
            // put back on end and activate
            _pars.push_back(p);
            p->_a._active = true;
        }

        return p;
    }

    void animOp(ParAnim* p)
    {
        assert(p);
        assert(p->_a.loaded());

        SSpineCtx* ctx = SSpineCtx::get();
        if (!ctx->animOp(p->_a)) // marks as done
        {
            LOG1(TAG_IMTEXT "animation op failed ", p->_a.name());
        }
    }

    void setAnimation(AnimInfo* ai)
    {
        // consume ai
        // look for existing animation and bump to last
        
        ParAnim* p = bump(ai);
        
        if (ai->_op)
        {
            // ops operate immediately
            if (p)
            {
                p->_a.setInfo(ai);  // overwrite current info
                animOp(p);
            }
            else
            {
                LOG1(TAG_IMTEXT "animation not found for op, ", ai->_name);
            }
        }
        else
        {
            if (!p)
            {
                p = new ParAnim(this);

                // bump will have disabled all others
                p->_a._active = true;
                _add(p);
            }

            // either load now or queue
            p->_a.queueInfo(ai);

            // kick off the loading (if not present)
            p->_a.load();
        }
    }

    static void render_anim(const ImDrawList* dl, const ImDrawCmd* cmd)
    {
        assert(cmd);
        ParAnim* pa = (ParAnim*)cmd->UserCallbackData;
        assert(pa);

        // first set the viewport rectangle to render in, same as
        // the ImGui draw command's clip rect
        int cx = (int) cmd->ClipRect.x;
        int cy = (int) cmd->ClipRect.y;
        int cw = (int) (cmd->ClipRect.z - cmd->ClipRect.x);
        int ch = (int) (cmd->ClipRect.w - cmd->ClipRect.y);

        //ImDrawData* dd = ImGui::GetDrawData();
        //int dx = dd->DisplayPos.x;
        //int dy = dd->DisplayPos.y;

        float dpi = sapp_dpi_scale();

        cx *= dpi;
        cy *= dpi;
        cw *= dpi;
        ch *= dpi;

        SSpineCtx* ctx = SSpineCtx::get();
        int ni = ctx->render(pa->_a);

        if (ni > 0)
        {
            sg_apply_scissor_rect(cx, cy, cw, ch, true);
            sg_apply_viewport(cx, cy, cw, ch, true);

            //LOG1(TAG_IMTEXT "view x:", cx << " y:" << cy << " w:" << cw << " h:" << ch << " dx:" << dx << " dy:" << dy << " dpi: " << dpi);

            // attach texture
            assert(pa->_a._tex && pa->_a._tex->valid());
            sg_image sgi{ (uint32_t)pa->_a._tex->_tid };
            state.anim.bind.fs_images[SLOT_tex] = sgi;

            sg_apply_pipeline(state.anim.pip);
            sg_apply_bindings(&state.anim.bind);

            float sx = 2.0f/pa->_a._skel_w;
            float sy = 2.0f/pa->_a._skel_h;

            vs_params_t vs_params;
            vs_params.offset[0] = -pa->_a._skel_x * sx - 1.0f;
            vs_params.offset[1] = -pa->_a._skel_y * sy - 1.0f;
            
            vs_params.scale[0] = sx;
            vs_params.scale[1] = sy;

            sg_range sgr1 = SG_RANGE(vs_params);
            sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_vs_params, &sgr1);
            sg_draw(0, ni, 1);
        }
    }
#endif  // USESPINE

protected:
    
    static void _formatCallback(const ImGui::MarkdownFormatInfo& info,
                                bool start)
    {
        assert(info.config);
        ImText* h = (ImText*)info.config->userData;
        assert(h);
        h->formatCallback(info, start);
    }

    static ImGui::MarkdownImageData _imageCallback(ImGui::MarkdownLinkCallbackData d)
    {
        ImText* h = (ImText*)d.userData;
        assert(h);
        return h->imageCallback(d);
    }
    
    static void _linkCallback(ImGui::MarkdownLinkCallbackData d)
    {
        ImText* h = (ImText*)d.userData;
        assert(h);
        h->linkCallback(d);
    }

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
