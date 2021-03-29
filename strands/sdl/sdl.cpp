// Dear ImGui: standalone example application for Emscripten, using SDL2 + OpenGL3
// (Emscripten is a C++-to-javascript compiler, used to publish executables for the web. See https://emscripten.org/)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

// This is mostly the same code as the SDL2 + OpenGL3 example, simply with the modifications needed to run on Emscripten.
// It is possible to combine both code into a single source file that will compile properly on Desktop and using Emscripten.
// See https://github.com/ocornut/imgui/pull/2492 as an example on how to do just that.

#define HAVE_FREETYPETEST

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include <stdio.h>
#include <emscripten.h>
#include <SDL.h>
#include <SDL_opengles2.h>
#include <SDL_mixer.h>
#include <emscripten/fiber.h>

// forward
const char* gui_input_pump();
void play_audio(const char* name);

#include "ifisdl.h"
#include "freetypetest.h"

SDL_Window*     g_Window = NULL;
SDL_GLContext   g_GLContext = NULL;
static float    fontSize = 24;
static bool     audioEnabled = true;

// For clarity, our main loop code is declared at the end.
static void main_loop();

void StrandWindow(bool* open);
void StrandInit();


struct Fiber {
    emscripten_fiber_t context;
    char asyncify_stack[1024*16];
    alignas(16) char c_stack[1024*64];
    int result = 0;

    void init_with_api(em_arg_callback_func entry, void *arg) {
        emscripten_fiber_init(&context, entry, arg, c_stack, sizeof(c_stack), asyncify_stack, sizeof(asyncify_stack));
    }

    void init_manually(em_arg_callback_func entry, void *arg) {
        context.stack_base = c_stack + sizeof(c_stack);
        context.stack_limit = c_stack;
        context.stack_ptr = context.stack_base;
        context.entry = entry;
        context.user_data = arg;
        context.asyncify_data.stack_ptr = asyncify_stack;
        context.asyncify_data.stack_limit = asyncify_stack + sizeof(asyncify_stack);
    }

    void swap(emscripten_fiber_t *fiber) {
        emscripten_fiber_swap(&context, fiber);
    }
};

static struct Globals
{
    emscripten_fiber_t main;
    char asyncify_stack[1024*16];
    Fiber fibers[1];

    Globals()
    {
        emscripten_fiber_init_from_current_context(&main, asyncify_stack, sizeof(asyncify_stack));
    }
} G;

static bool strand_done = false;

void runfiber(void* arg)
{
    LOG1("starting strand", "");
    StrandInit();

    // done, so switch back to main
    strand_done = true;
    G.fibers[0].swap(&G.main);
}


void strand_pump()
{
    // switch to strand
    if (!strand_done)
        emscripten_fiber_swap(&G.main, &G.fibers[0].context);
}

int main(int, char**)
{
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO
                 | SDL_INIT_TIMER
                 | SDL_INIT_GAMECONTROLLER
                 | SDL_INIT_AUDIO) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }


    int frequency, active_flags;

    frequency = EM_ASM_INT_V({
            var context;
            try {
                context = new AudioContext();
            } catch (e) {
                context = new webkitAudioContext(); // safari only
            }
            return context.sampleRate;
	});

      
    if (Mix_OpenAudio(frequency, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
        printf("Failed to Mix_OpenAudio(): %s\n", Mix_GetError());
        return -1;
    }

    int mix_flags = MIX_INIT_OGG;
    active_flags = Mix_Init(mix_flags);
    
    if ((mix_flags & active_flags) != mix_flags) {
        printf("Failed to Mix_Init(): %s\n", Mix_GetError());
        return -1;
    }

    const char* glsl_version = "#version 100";
    //const char* glsl_version = "#version 300 es";

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);
    
    int window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    window_flags += SDL_WINDOW_ALLOW_HIGHDPI;
    
    g_Window = SDL_CreateWindow("Strand",
                                SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED,
                                1280, 720,
                                (SDL_WindowFlags)window_flags);
    
    g_GLContext = SDL_GL_CreateContext(g_Window);
    if (!g_GLContext)
    {
        fprintf(stderr, "Failed to initialize WebGL context!\n");
        return 1;
    }
    
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
    // You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
    io.IniFilename = NULL;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(g_Window, g_GLContext);
    ImGui_ImplOpenGL3_Init(glsl_version);

    ImFontConfig config;
    
    // these oversampling values are ignored by freetype atlas build
    //config.OversampleH = 4;
    //config.OversampleV = 2;
    //config.GlyphExtraSpacing.x = 1.0f;
    io.Fonts->AddFontFromFileTTF("fonts/Roboto-Regular.ttf", fontSize);
    io.Fonts->AddFontFromFileTTF("fonts/Merriweather-Light.ttf", fontSize);

    G.fibers[0].init_with_api(runfiber, 0);
    emscripten_set_main_loop(main_loop, 0, true);

    play_audio(0);  // stop and free music
    Mix_CloseAudio();
}

static void main_loop()
{
    ImGuiIO& io = ImGui::GetIO();

    // Our state (make them static = more or less global) as a convenience to keep the example terse.
    static bool show_demo_window = false;
    static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    for (;;)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            // Capture events here, based on io.WantCaptureMouse and io.WantCaptureKeyboard
        }

        bool v = ImGui_ImplSDL2_Tick();
        if (v) break;

        SDL_Delay(0);
    }

    if (ImGui::IsKeyPressed(0x3A)) show_demo_window = true;

    // debug show freetype tweak window
    static bool show_freetype = false;
    if (ImGui::IsKeyPressed(0x3B)) show_freetype = true;

    // if font changed
    if (freetype_test.PreNewFrame(fontSize))
    {
        // REUPLOAD FONT TEXTURE TO GPU
        ImGui_ImplOpenGL3_DestroyDeviceObjects();
        ImGui_ImplOpenGL3_CreateDeviceObjects();
    }

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(g_Window);
    ImGui::NewFrame();

    if (show_freetype)
        freetype_test.ShowFontsOptionsWindow(&show_freetype);

    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    strand_pump();

    static bool show_strand = true;
    if (show_strand) StrandWindow(&show_strand);

    // Rendering
    ImGui::Render();
    SDL_GL_MakeCurrent(g_Window, g_GLContext);
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(g_Window);
}

static StrandCtx sctx;

const char* gui_input_pump()
{
    // called from strand to poll for input
    const char* label;
    const char* s = sctx.yieldCmd(&label);
    if (s)
    {
        sctx._mainText.add(label, true);
        return s; // something to do
    }
    else
    {
        // yield back to main
        G.fibers[0].swap(&G.main);
    }
    return 0;
}

static void textReceiver(const char* s)
{
    sctx._mainText.add(s);
}

void StrandInit()
{
    using namespace std::placeholders;  
    SDLHandler::TextEmitter e = std::bind(textReceiver, _1);
    bool v = sctx.init(e);
    LOG3("StrandInit Complete, ", v);
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


void PopKeyboard()
{
    /* doesnt work!
    EM_ASM({
            var canvas = document.getElementById('canvas');
            canvas.contentEditable=true;
  });
    */

}

static const char* fontName(ImFont* f)
{
    static char buf[64];
    const char* p = f->GetDebugName();
    char* q = buf;
    while (*p)
    {
        if (*p == '.') break;
        *q++ = *p++;
    }
    *q = 0;
    return buf;
}

static void fontMenu()
{
    float h = ImGui::GetFrameHeightWithSpacing();
    float w = 20*fontSize;
    if (w < 400) w = 400;
    ImGui::BeginChild("fontchooser", ImVec2(w,h), false);
    ImGuiIO& io = ImGui::GetIO();
    
    ImFont* font_current = ImGui::GetFont();

    ImGui::SetNextItemWidth(w/2);
    if (ImGui::BeginCombo("Font", fontName(font_current)))
    {
        for (int n = 0; n < io.Fonts->Fonts.Size; n++)
        {
            ImFont* font = io.Fonts->Fonts[n];
            ImGui::PushID((void*)font);
            if (ImGui::Selectable(fontName(font), font == font_current))
                io.FontDefault = font;
            ImGui::PopID();
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
        freetype_test.WantRebuild = true;
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

    ImGuiIO& io = ImGui::GetIO();

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
                if (!audioEnabled) play_audio(0); // kill 
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

        static bool popper = false;
        
        if (reclaim_focus)
        {
            popper = false;
            ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget
        }
        
        if (ImGui::IsItemFocused())
        {
            if (!popper)
            {
                popper = true;
                PopKeyboard();
            }
        }
        else popper = false;
        
        ImGui::EndChild();
    }

    ImGui::End();
}

void play_audio(const char* fname)
{
    static Mix_Music* m = 0;
    
    if (m)
    {
        Mix_FreeMusic(m);
        m = 0;
    }

    if (fname && audioEnabled)
    {
        m = Mix_LoadMUS(fname);
        if (m)
        {
            if (Mix_PlayMusic(m, 1))
            {
                LOG1("Unable to play audio ", fname);
                Mix_FreeMusic(m);
                m = 0;
            }
        }
        else
        {
            LOG1("Unable to load audio ", fname);
        }
    }
}

