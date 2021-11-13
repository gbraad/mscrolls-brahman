
#define SOKOL_IMPL
//#define SOKOL_GLES2

#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_time.h"
#include "sokol_glue.h"
#include "imgui.h"
#include "sokol_imgui.h"
#include "sokol_audio.h"
#include "sokol_fetch.h"
#include "sokol_args.h"

// generated shader for animation
#include "imgui-anim.glsl.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO
#define STBI_NO_BMP
#define STBI_NO_TGA
#define STBI_NO_GIF
#include "stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION

#define STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.c"

#include <vector>
#include "logged.h"

static bool show_demo_window = false;
static bool show_quit_dialog = false;
static bool html5_ask_leave_site = false;
static bool strand_enabled = false;
static bool show_strand = true;
static bool audioEnabled = true;
static bool fontsChanged = true;
static float fontSize = 24;

struct FontFileInfo
{
    std::string         _name;
    bool                _bold = false;
    bool                _italic = false;
    ImFont*             _imFont = 0;

    FontFileInfo(const std::string& name) : _name(name) {}
};

static std::vector<FontFileInfo> fontFiles;


static const char* requestFontList;
static std::string requestFontName;
static std::string requestStoryName;
static std::string requestAudioName;
static sg_pass_action pass_action;

#include "imgui_internal.h"
#include <stdio.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/fiber.h>
#endif


// forward
std::string gui_input_pump();
void play_audio(const char* name);
void stop_audio();
void show_picture(const std::string&);

#include "fetcher.h"

Fetcher fetcher;

#include "imtex.h"

ImTexLoader texLoader(fetcher);


#ifdef USESPINE
#include "sspine.h"
#endif

#include "ifisdl.h"

void StrandWindow(bool* open);
void StrandInit(const char*);


#ifdef __EMSCRIPTEN__

#define ASTACK_SZ 1024*16
#define FIBSTACK_SZ 1024*64

struct Fiber
{
    emscripten_fiber_t context;
    char asyncify_stack[ASTACK_SZ];
    alignas(16) char c_stack[FIBSTACK_SZ];

    void init_with_api(em_arg_callback_func entry, void *arg)
    {
        emscripten_fiber_init(&context,
                              entry, arg,
                              c_stack, sizeof(c_stack),
                              asyncify_stack, sizeof(asyncify_stack));
    }

    /*
    void init_manually(em_arg_callback_func entry, void *arg)
    {
        context.stack_base = c_stack + sizeof(c_stack);
        context.stack_limit = c_stack;
        context.stack_ptr = context.stack_base;
        context.entry = entry;
        context.user_data = arg;
        context.asyncify_data.stack_ptr = asyncify_stack;
        context.asyncify_data.stack_limit = asyncify_stack + sizeof(asyncify_stack);
    }
    */

    void swap(emscripten_fiber_t *fiber)
    {
        emscripten_fiber_swap(&context, fiber);
    }
};

struct Globals
{
    emscripten_fiber_t _main;
    char _asyncify_stack[ASTACK_SZ];
    Fiber fibers[1];

    Globals()
    {
        emscripten_fiber_init_from_current_context(&_main,
                                                   _asyncify_stack,
                                                   sizeof(_asyncify_stack));
    }
};

static Globals* G;

void runfiber(void* arg)
{
    if (strand_enabled)
    {
        LOG1("starting strand ", requestStoryName);
        StrandInit(requestStoryName.c_str());

        // strand is done, so disable.
        strand_enabled = false;
    }

    // so switch back to main
    G->fibers[0].swap(&G->_main);
}


void strand_pump()
{
    // switch to strand
    if (strand_enabled)
        emscripten_fiber_swap(&G->_main, &G->fibers[0].context);
}

#endif // __EMSCRIPTEN__

static StrandCtx sctx;

std::string gui_input_pump()
{
    // called from strand to poll for input
    const char* label;
    std::string s;
    if (sctx.yieldCmd(s, &label))
    {
        sctx._mainText.add(label, true);
    }
    else
    {
        // yield back to main
        G->fibers[0].swap(&G->_main);
    }
    return s;
}

static void textReceiver(const char* s)
{
    sctx._mainText.add(s);
}

void StrandInit(const char* story)
{
    using namespace std::placeholders;  
    SDLHandler::TextEmitter e = std::bind(textReceiver, _1);
    bool v = sctx.init(e, story);
}

static int InputCallback(ImGuiInputTextCallbackData* data)
{
    if (data->EventFlag == ImGuiInputTextFlags_CallbackCompletion)
    {
        data->InsertChars(data->CursorPos, "..");
    }
    else if (data->EventFlag == ImGuiInputTextFlags_CallbackHistory)
    {
        if (data->EventKey == ImGuiKey_UpArrow)
        {
            const char* s = sctx._hist.up();
            if (s)
            {
                data->DeleteChars(0, data->BufTextLen);
                data->InsertChars(0, s);
            }
            //data->SelectAll();
        }
        else if (data->EventKey == ImGuiKey_DownArrow)
        {
            const char* s = sctx._hist.down();
            if (s)
            {
                data->DeleteChars(0, data->BufTextLen);
                data->InsertChars(0, s);
                //data->SelectAll();
            }
        }
    }
    return 0;
}

static void setF(const char* name)
{
    ImGuiID id;

    if (name) id = ImGui::GetID(name);
    else id = ImGui::GetItemID();
    
    // https://github.com/ocornut/imgui/issues/1712
    ImGui::SetFocusID(id, ImGui::GetCurrentWindow()); 
    //ImGuiContext& g = *ImGui::GetCurrentContext();
    //g.NavDisableHighlight = false;
}


static const char* fontName(ImFont* f)
{
    const char* p = f->GetDebugName();
    return p;
}

static void fontMenu()
{
    float h = ImGui::GetFrameHeightWithSpacing();
    float w = 20*fontSize;
    if (w < 400) w = 400;
    ImGui::BeginChild("fontchooser", ImVec2(w,h), false);
    //ImGuiIO& io = ImGui::GetIO();

    ImGui::SetNextItemWidth(w/2);
    ImFont* font_current = ImGui::GetFont();
    if (ImGui::BeginCombo("Font", fontName(font_current)))
    {
        for (int i = 0; i < fontFiles.size(); ++i)
        {
            const FontFileInfo& ffi = fontFiles[i];
            if (!ffi._bold && !ffi._italic)
            {
                const char* fi = ffi._name.c_str();
                if (ImGui::Selectable(fi)) requestFontName = fi;
            }
        }

        ImGui::EndCombo();
    }
    ImGui::SameLine();

    ImGui::SetNextItemWidth(w/4);
    int fz = fontSize;
    ImGui::InputInt("Size", &fz, 1, 0);
    if (fz > 40) fz = 40;
    else if (fz < 12) fz = 12;
    if (fontSize != fz)
    {
        fontSize = fz;
        fontsChanged = true;
    }
    
    ImGui::EndChild();
}


void StrandWindow(bool* strand_open)
{
    ImGuiWindowFlags flags = 0;

    flags |= ImGuiWindowFlags_MenuBar;
    flags |= ImGuiWindowFlags_NoCollapse;

#if 0
    // windowed    
    ImGui::SetNextWindowSize(ImVec2(800, 720), ImGuiCond_FirstUseEver);
#else
    // full screen
    bool use_work_area = true;
    flags |= ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings;

    //flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings;


    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(use_work_area ? viewport->WorkPos : viewport->Pos);
    ImGui::SetNextWindowSize(use_work_area ? viewport->WorkSize : viewport->Size);
#endif    

    //ImGuiIO& io = ImGui::GetIO();

    ImGuiStyle& style = ImGui::GetStyle();
    style.ScrollbarSize = 24.0f;  // fat enough for mobile
    style.ChildRounding = 4.0f;
    
    ImVec2 pad = style.FramePadding;
    ImVec2 spacing = style.ItemSpacing;

    // dont pass in show flag
    std::string tbar = sctx.h.getGameTitle();
    if (tbar.empty()) tbar = "Strand";
    ImGui::Begin(tbar.c_str(), 0, flags);

    if (ImGui::BeginMenuBar())
    {
#if 0
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
            if (ImGui::MenuItem("Save", "Ctrl+S"))   { /* Do stuff */ }
            if (ImGui::MenuItem("Close", "Ctrl+W"))  { *strand_open = false; }
            ImGui::EndMenu();
        }
#endif

        if (ImGui::BeginMenu("Style"))
        {
            if (ImGui::MenuItem("Dark")) ImGui::StyleColorsDark();
            if (ImGui::MenuItem("Light")) ImGui::StyleColorsLight();
            if (ImGui::MenuItem("Classic")) ImGui::StyleColorsClassic();
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Font"))
        {
            fontMenu();
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Options"))
        {
            if (ImGui::MenuItem("Sound", NULL, audioEnabled))
            {
                audioEnabled = !audioEnabled;
                if (!audioEnabled) stop_audio();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    // get client space
    //ImVec2 top = ImGui::GetCursorPos();
    ImVec2 sz = ImGui::GetContentRegionAvail();

    ImVec2 titleBoxSz(sz.x, ImGui::GetFrameHeightWithSpacing() + pad.y);

    bool inputActive = true;
    int cby = 0;
    int txy = sz.y - (titleBoxSz.y + spacing.y);
    
    IFIHandler::ChoicesInfo* ch = sctx.h._choice;
    if (ch)
    {
        int n = ch->size();
        if (ch->_header) ++n;
        
        int lh = ImGui::GetFrameHeightWithSpacing();

        // choice box can switch off input
        inputActive = ch->_textinput;

        cby = lh*n;
        cby += pad.y*2 + spacing.y; // whatever!
        txy -= cby + spacing.y;
    }

    ImVec2 inputBoxSz(sz.x, ImGui::GetFontSize() * 2);  // if shown

    if (inputActive)
    {
        txy -= inputBoxSz.y + spacing.y;
    }
    
    ImVec2 choiceBoxSz(sz.x, cby);
    ImVec2 textBoxSz(sz.x, txy);


    // title box
    const char* titleText = sctx.getTitle();
    int f = ImGuiWindowFlags_NoScrollbar;
    ImGui::BeginChild("Title", titleBoxSz, true, f);
    bool alignRight = false;
    if (alignRight)
    {
        int xr = titleBoxSz.x - pad.x - style.ScrollbarSize;
        xr -= ImGui::CalcTextSize(titleText).x;
        ImGui::SetCursorPosX(xr);
    }
    ImGui::TextColored(ImVec4(1,1,0,1), "%s", titleText);
    ImGui::EndChild();

    if (txy > 0)
    {
        // main text box
        ImGui::BeginChild("Main", textBoxSz, true);

        static bool requestScroll = false;

        // render our text, a block at a time
        sctx._mainText.render();
        
        if (requestScroll)
        {
            ImGui::SetScrollHereY(1);
            requestScroll = false;
        }

        if (sctx._mainText._changed)
        {
            // if changed, set flag to scroll on NEXT frame.
            requestScroll = true;
            sctx._mainText._changed = false;
        }
        
        ImGui::EndChild();
    }

    // choice box
    if (cby > 0)
    {
        int f = ImGuiWindowFlags_NoScrollbar;
        ImGui::BeginChild("choice", choiceBoxSz, true, f);

        if (ch->_header)
        {
            ImGui::TextColored(ImVec4(1,1,0,1), "%s",
                               ch->_header._text.c_str());
        }

        int n = ch->size();

        for (int i = 0; i < n; ++i)
        {
            const std::string& t = ch->_choices[i]._text._text;
            
            bool accept = false;
            
            //int f = ImGuiSelectableFlags_AllowDoubleClick;
            //accept = ImGui::Selectable(t.c_str(), ch->_selected == i, f);

            if (ImGui::RadioButton(t.c_str(), ch->_selected == i))
            {
                accept = true;
            }

            if (ImGui::IsItemFocused())
            {
                ch->_selected = i;
                if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter)))
                {
                    accept = true;
                }
            }

            if (accept)
            {
                ch->_selected = i;

                char buf[8];
                sprintf(buf, "%d", i+1);
                sctx.sendCmd(buf, &t);
            }
        }

        if (ch->_selected < 0 && !inputActive)
        {
            // none have focus, force the first one
            ch->_selected = 0;
            const std::string& t = ch->_choices[0]._text._text;
            setF(t.c_str());
        }
        
        ImGui::EndChild();
    }

    if (inputActive)
    {
        // input box
        static char buf1[128];
        ImGui::BeginChild("Input", inputBoxSz, true);
        ImGui::SetNextItemWidth(inputBoxSz.x - pad.x*2);

        int tf =
            ImGuiInputTextFlags_CallbackCompletion
            | ImGuiInputTextFlags_CallbackHistory
            //| ImGuiInputTextFlags_CallbackEdit
            ;

        tf |=  ImGuiInputTextFlags_EnterReturnsTrue ;
        
        bool reclaim_focus = false;
        static bool claimed = false;

        if (ImGui::InputTextWithHint("", "type here", buf1, sizeof(buf1),
                                     tf, InputCallback))
        {
            sctx.sendCmd(buf1);
            *buf1 = 0; // clear
            reclaim_focus = true;
        }

        if (cby)
        {
            // allow claim next time no choices
            claimed = false;
        }
        else
        {
            // claim focus just the once 
            if (!claimed)
            {
                reclaim_focus = true;
                claimed = true;
            }
        }

        if (reclaim_focus)
            ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget
        
        ImGui::EndChild();
    }

    ImGui::End();
}

void show_picture(const std::string& fname)
{
    sctx._mainText.addImage(fname);
}

void play_audio(const char* fname)
{
    if (audioEnabled)
        requestAudioName = fname;
}

void stop_audio()
{
    ;
}

void play_animation(AnimInfo* ai)
{
#ifdef USESPINE
    LOG1("play_animation ", ai->_name << " " << ai->_play);

    // no point in loading otherwise
    sctx._mainText.setAnimation(ai);
#endif    
}

static void makeFonts()
{
    // create font texture for the custom font

    if (fontsChanged)
    {
        fontsChanged = false;

        auto& io = ImGui::GetIO();
        io.Fonts->ClearTexData();

        for (int i = 0; i < io.Fonts->Fonts.Size; ++i)
        {
            ImFontConfig& ci = io.Fonts->ConfigData[i];
            ci.SizePixels = fontSize;
        }
    
        unsigned char* font_pixels;
        int font_width, font_height;
        io.Fonts->GetTexDataAsRGBA32(&font_pixels, &font_width, &font_height);
        //printf("font w:%d, h:%d\n", font_width, font_height);
        sg_image_desc img_desc = { };
        img_desc.width = font_width;
        img_desc.height = font_height;
        img_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
        img_desc.wrap_u = SG_WRAP_CLAMP_TO_EDGE;
        img_desc.wrap_v = SG_WRAP_CLAMP_TO_EDGE;
        img_desc.min_filter = SG_FILTER_LINEAR;
        img_desc.mag_filter = SG_FILTER_LINEAR;
        img_desc.data.subimage[0][0].ptr = font_pixels;
        img_desc.data.subimage[0][0].size = font_width * font_height * 4;
        io.Fonts->TexID = (ImTextureID)(uintptr_t) sg_make_image(&img_desc).id;
    }
}

static std::string fontStem(const char* name)
{
    // remove any final "-bold" etc 
    std::string stem;
    const char* p = strrchr(name, '-');
    if (p)
    {
        stem = std::string(name, (p-name));
    }
    else stem = name;
    return stem;
}

static inline std::string fontStem(const std::string& name)
{ return fontStem(name.c_str()); }

static FontFileInfo* findFontInfo(const std::string& name)
{
    // find the exact info
    std::string fn = filenameOf(name);
    for (auto& ffi : fontFiles)
        if (ffi._name == fn) return &ffi;
            
    return 0;
}

static FontFileInfo* findFontInfoFor(const std::string& name,
                                     bool bold, bool italic)
{
    std::string sname = fontStem(filenameOf(name));
    for (auto& ffi : fontFiles)
    {
        if (fontStem(ffi._name) == sname
            && ffi._bold == bold
            && ffi._italic == italic) return &ffi;
    }
            
    return 0;
}

static void font_list_loaded(const char* name, void* ctx)
{
    if (fetcher.ok)
    {
        ImGuiIO& io = ImGui::GetIO();
    
        int sz;
        char* fdata = fetcher.yield(sz);

        // expect fontlist.txt in unix format (not DOS)
        std::vector<std::string> fonts;
        split(fonts, fdata, '\n');

        int baseFont = 0;

        for (int i = 0; i < fonts.size(); ++i)
        {
            std::string& fname = fonts[i];
            FontFileInfo ffi(fname);
            std::string f = toLower(fname);
            if (f.find("bold") != std::string::npos) ffi._bold = true;
            if (f.find("italic") != std::string::npos) ffi._italic = true;

            if (!ffi._bold && !ffi._italic && f.find("roboto") != std::string::npos) baseFont = i;

            fontFiles.push_back(ffi);
        }

        LOG1("Loaded font list ", fdata << " entries " << fontFiles.size());

        if (fontFiles.size())
        {
            // kick off loading of the first font
            requestFontName = fontFiles[baseFont]._name;
        }

        delete fdata;
    }
}

static void font_loaded(const char* name, void* ctx)
{
    if (fetcher.ok)
    {
        ImGuiIO& io = ImGui::GetIO();
    
        LOG1("Loaded font ", name);

        FontFileInfo* ffi = findFontInfo(name);
        assert(ffi);

        FontFileInfo* ffn = 0;

        // if loading regular -> load bold
        // if loading bold -> load italic
        // if loading italic -> done

        if (!ffi->_italic)
        {
            if (ffi->_bold)
            {
                // load italic (if exists)
                ffn = findFontInfoFor(name, false, true);
            }
            else
            {
                // load bold
                ffn = findFontInfoFor(name, true, false);

                io.Fonts->Clear();
                for (auto& fi : fontFiles) fi._imFont = 0;
                sctx._mainText.setBoldFont(0);
                sctx._mainText.setItalicFont(0);
            }
        }

        if (ffn)
        {
            // another font of same family, so trigger load
            requestFontName = ffn->_name;
        }

        int sz;
        char* fdata = fetcher.yield(sz);
        
        ImFontConfig cfg;
        const char* p;
        for (p = name + strlen(name); p > name && p[-1] != '/' && p[-1] != '\\'; p--) {}
        ImFormatString(cfg.Name, IM_ARRAYSIZE(cfg.Name), "%s, %.0fpx", p, fontSize);

        ImFont* imf =
            io.Fonts->AddFontFromMemoryTTF((void*)fdata, sz, fontSize, &cfg);

        ffi->_imFont = imf;
        if (ffi->_bold && !ffi->_italic) sctx._mainText.setBoldFont(imf);
        if (!ffi->_bold && ffi->_italic) sctx._mainText.setItalicFont(imf);

        fontsChanged = true;
    }
}

struct AudioState
{
    static const int bufSize = 64*1024;

    struct Buf
    {
        float*  _f = 0;
        int     _size;
        int     _pos = 0;
        Buf*    _next = 0;
        
        ~Buf() { delete _f; }
    };

    Buf*        _bufs = 0;
    Buf**       _tail = &_bufs;

    int         _output_sample_rate;
    int         _output_channels;

    ~AudioState() { purge(); }

    Buf* _add(int n)
    {
        Buf* b = new Buf;
        b->_size = n;
        b->_f = new float[n];
        *_tail = b;
        _tail = &b->_next;
        return b;
    }

    void pop()
    {
        // ASSUME non-empty
        _bufs = _bufs->_next;
        if (!_bufs) _tail = &_bufs;
    }

    void add(int n, float* data)
    {
        Buf* b = _add(n);
        memcpy(b->_f, data, n*sizeof(*data));
    }

    void add(int n, float* ldata, float* rdata)
    {
        // mix left + right to mono
        Buf* b = _add(n);
        float* dp = b->_f;
        for (int i = 0; i < n; ++i)
            *dp++ = ((*ldata++) + (*rdata++))/2;
    }

    void purge()
    {
        while (_bufs)
        {
            Buf* b = _bufs->_next;
            delete _bufs;
            _bufs = b;
        }
    }
    
};

static AudioState audioState;

static void audio_loaded(const char* name, void* ctx)
{
    if (fetcher.ok)
    {
        int sz;
        char* fdata = fetcher.yield(sz);

        LOG1("Loaded audio ", name << " data size:" << sz);

        int error;
        stb_vorbis* v = stb_vorbis_open_memory((const unsigned char*)fdata, sz,
                                               &error, NULL);

        if (v)
        {
            stb_vorbis_info vi = stb_vorbis_get_info(v);
            LOG1("audio ", name << " nchan:" << vi.channels << " sample rate:" << vi.sample_rate);

            for (;;)
            {
                float** out;
                int nsamples = stb_vorbis_get_frame_float(v, NULL, &out);

                if (!nsamples) break; //done

                if (audioState._output_channels == 1)
                {
                    // mix down to mono
                    if (vi.channels == 1)
                    {
                        //LOG1("audio push buffer ", nsamples);
                        audioState.add(nsamples, out[0]);
                    }
                    else if (vi.channels == 2)
                    {
                        //LOG1("audio mix to mono ", name << " " << nsamples);
                        audioState.add(nsamples, out[0], out[1]);
                    }
                }
                else //if (audioState._output_channels == 2)
                {
                    LOG1("audio output channels not supported ",
                         audioState._output_channels);
                    break;
                }
            }

            stb_vorbis_close(v);
        }
        else
        {
            LOG1("vorbis cannot decode ", name << " error:" << error);
        }
        

        delete fdata;
    }
}

static void loadFontList()
{
    if (requestFontList)
    {
        if (fetcher.start(requestFontList, font_list_loaded))
            requestFontList = 0;
    }
}

static void loadFonts()
{
    if (!requestFontName.empty())
    {
        // expect all fonts to be in "fonts" subdir
        std::string fpath = "fonts/" + requestFontName;
        if (fetcher.start(fpath, font_loaded))
            requestFontName.clear();
    }
}

static void loadAudio()
{
    if (!requestAudioName.empty())
    {
        LOG1("requesting audio ", requestAudioName);
        if (fetcher.start(requestAudioName, audio_loaded))
            requestAudioName.clear();
    }
}

static void pumpAudio()
{
    // using the push model, we feed samples
    int num_frames = saudio_expect();
    if (num_frames > 0)
    {
        int num_samples = num_frames * saudio_channels();

        while (num_samples)
        {
            AudioState::Buf* b = audioState._bufs;
            if (!b) break;

            int r = b->_size - b->_pos;
            if (r > num_samples) r = num_samples;
            
            saudio_push(b->_f + b->_pos, r);
            b->_pos += r;
            num_samples -= r;

            if (b->_pos == b->_size)
            {
                // all used
                audioState.pop();
                delete b;
            }
        }
    }
}

SGState state;

void init()
{
    // setup sokol-gfx and sokol-time
    sg_desc desc = { };
    desc.context = sapp_sgcontext();
    sg_setup(&desc);

    // requested 
    saudio_desc adesc = {};
    adesc.sample_rate = 44100; // also default
    adesc.num_channels = 1; // mono
    //adesc.stream_cb = audio_cb;
    //adesc.buffer_frames = 4096;  // default is 2048, make bigger
    adesc.stream_cb = 0; // push model
    saudio_setup(&adesc);

    // actual
    audioState._output_sample_rate = saudio_sample_rate();
    audioState._output_channels = saudio_channels();

    sfetch_desc_t fdesc = {};
    fdesc.max_requests = 1;
    fdesc.num_channels = 1;
    fdesc.num_lanes = 1;
    sfetch_setup(&fdesc);

    // setup sokol-imgui, but provide our own font
    simgui_desc_t simgui_desc = { };
    simgui_desc.no_default_font = true;
    simgui_desc.dpi_scale = sapp_dpi_scale();
    simgui_setup(&simgui_desc);

    // file with list of fonts on server
    requestFontList = "fontlist.txt";
    
    // configure Dear ImGui with our own embedded font
    //    auto& io = ImGui::GetIO();
    
    //ImFontConfig fontCfg;
    //fontCfg.FontDataOwnedByAtlas = false;
    //fontCfg.OversampleH = 2;
    //fontCfg.OversampleV = 2;
    //fontCfg.RasterizerMultiply = 1.5f;
    
    //io.Fonts->AddFontFromFileTTF("fonts/Roboto-Regular.ttf", fontSize);
    //io.Fonts->AddFontFromFileTTF("fonts/Merriweather-Light.ttf", fontSize);

    // initial clear color
    pass_action.colors[0].action = SG_ACTION_CLEAR;
    pass_action.colors[0].value = { 0.45f, 0.55f, 0.6f, 1.0f };

#ifdef USESPINE    
    static sg_buffer_desc sgb = {};

    // supply data later
    sgb.size = MAX_SVERTICES*sizeof(SVertex);
    sgb.usage = SG_USAGE_STREAM;
    //sgb.label = "vertices";
    state.anim.bind.vertex_buffers[0] = sg_make_buffer(&sgb);

    static sg_buffer_desc sgb2 = {};
    sgb2.type = SG_BUFFERTYPE_INDEXBUFFER;
    sgb2.size = (MAX_SVERTICES*3/2)*sizeof(unsigned short);
    sgb2.usage = SG_USAGE_STREAM;
    //sgb2.label = "indices";
    state.anim.bind.index_buffer = sg_make_buffer(&sgb2);

    static sg_pipeline_desc sgpip = {};
    sgpip.layout.attrs[ATTR_vs_pos].format = SG_VERTEXFORMAT_FLOAT2;
    sgpip.layout.attrs[ATTR_vs_color0].format   = SG_VERTEXFORMAT_FLOAT4;
    sgpip.layout.attrs[ATTR_vs_texcoord0].format = SG_VERTEXFORMAT_FLOAT2;
    sgpip.shader =  sg_make_shader(scene_shader_desc(sg_query_backend()));
    sgpip.index_type = SG_INDEXTYPE_UINT16;
    sgpip.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;
    sgpip.depth.write_enabled = true;
    sgpip.cull_mode = SG_CULLMODE_BACK;

    sg_blend_state& bs = sgpip.colors[0].blend;
    bs.enabled = true;
    //bs.src_factor_rgb = SG_BLENDFACTOR_ONE;
    bs.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA;
    //bs.dst_factor_rgb = SG_BLENDFACTOR_ZERO;
    bs.dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    bs.src_factor_alpha = SG_BLENDFACTOR_ONE;
    bs.dst_factor_alpha = SG_BLENDFACTOR_ZERO;
        
    state.anim.pip = sg_make_pipeline(&sgpip);

    // set up things spine needs to load etc.
    SSpineCtx* ssc = SSpineCtx::get();
    ssc->_loader = &texLoader;
#endif // USESPINE    

    G = new Globals();
    G->fibers[0].init_with_api(runfiber, 0);

    strand_enabled = true; // start from main loop
}

void frame(void)
{
    const int width = sapp_width();
    const int height = sapp_height();

    loadFontList();
    loadFonts();
    loadAudio();
    texLoader.poll();  // use fetcher to load images
    fetcher.poll();
    makeFonts();

    pumpAudio();

    simgui_new_frame(width, height, 1.0/30.0); // 30 fps

    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    strand_pump();

    if (show_strand) StrandWindow(&show_strand);

    // the sokol_gfx draw pass
    sg_begin_default_pass(&pass_action, width, height);
    simgui_render();
    sg_end_pass();
    sg_commit();
}

void cleanup(void)
{
    sfetch_shutdown();
    saudio_shutdown();
    simgui_shutdown();
    sg_shutdown();
    sargs_shutdown();
}

void input(const sapp_event* event)
{
    if (event->type == SAPP_EVENTTYPE_QUIT_REQUESTED)
    {
        show_quit_dialog = true;
        sapp_cancel_quit();
    }
    else
    {
        if (event->type == SAPP_EVENTTYPE_KEY_DOWN)
        {
            if (event->key_code == SAPP_KEYCODE_F1) show_demo_window = true;
        }
        simgui_handle_event(event);
    }
}

extern "C" void reload_story(const char* name)
{
    // call on main fiber
    strand_enabled = false;
    
    LOG1("reload_story ", name);
    
    sctx.resetAll();  // drop IFI client

    requestStoryName = name;
    G->fibers[0].init_with_api(runfiber, 0);

    // runfiber will run from the main loop once enabled
    strand_enabled = true;
}

sapp_desc sokol_main(int argc, char* argv[])
{
    sargs_desc adesc = {};
    adesc.argc = argc;
    adesc.argv = argv;
    sargs_setup(&adesc);

    /*
    for (int i = 0; i < sargs_num_args(); i++)
        printf("key: %s, value: %s\n", sargs_key_at(i), sargs_value_at(i));
    */

    // have a "story" arg?
    const char* str = sargs_value("story");
    if (str) requestStoryName = str;

    sapp_desc desc = { };
    desc.init_cb = init;
    desc.frame_cb = frame;
    desc.cleanup_cb = cleanup;
    desc.event_cb = input;
    desc.width = 1024;
    desc.height = 768;
    desc.fullscreen = true;

    // renders full size on Hi DPI screens
    desc.high_dpi = true;
    desc.html5_ask_leave_site = html5_ask_leave_site;
    desc.ios_keyboard_resizes_canvas = false;
    //desc.icon.sokol_default = true;
    //desc.gl_force_gles2 = true;
    //desc.window_title = "Dear ImGui HighDPI (sokol-app)";
    return desc;
}

