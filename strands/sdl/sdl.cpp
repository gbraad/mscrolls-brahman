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
#include <emscripten/fiber.h>

#ifdef HAVE_FREETYPETEST
#include "freetypetest.h"
#endif

// forward
const char* gui_input_pump();
void addText(const char* s);

#include "ifisdl.h"


SDL_Window*     g_Window = NULL;
SDL_GLContext   g_GLContext = NULL;

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
    {
        emscripten_fiber_swap(&G.main, &G.fibers[0].context);
    }
}

int main(int, char**)
{
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
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
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(g_Window, g_GLContext);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    // - Emscripten allows preloading a file or folder to be accessible at runtime. See Makefile for details.
    //io.Fonts->AddFontDefault();
#ifndef IMGUI_DISABLE_FILE_FUNCTIONS
    //io.Fonts->AddFontFromFileTTF("fonts/Roboto-Regular.ttf", 18.0f);


    ImFontConfig config;

    float fsize = 24.0f;

    // these oversampling values are ignored by freetype atlas build
    //config.OversampleH = 4;
    //config.OversampleV = 2;
    //config.GlyphExtraSpacing.x = 1.0f;
    io.Fonts->AddFontFromFileTTF("fonts/Roboto-Regular.ttf", fsize);
    io.Fonts->AddFontFromFileTTF("fonts/Merriweather-Light.ttf", fsize);
#endif

    G.fibers[0].init_with_api(runfiber, 0);
    emscripten_set_main_loop(main_loop, 0, true);
}

static void main_loop()
{
    ImGuiIO& io = ImGui::GetIO();

    // Our state (make them static = more or less global) as a convenience to keep the example terse.
    static bool show_demo_window = false;
    static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL2_ProcessEvent(&event);
        // Capture events here, based on io.WantCaptureMouse and io.WantCaptureKeyboard
    }

    static bool show_freetype = false;

    if (ImGui::IsKeyPressed(0x3A)) show_demo_window = true;
    if (ImGui::IsKeyPressed(0x3B)) show_freetype = true;

#ifdef HAVE_FREETYPETEST    
    if (show_freetype && freetype_test.PreNewFrame())
    {
        // REUPLOAD FONT TEXTURE TO GPU
        ImGui_ImplOpenGL3_DestroyDeviceObjects();
        ImGui_ImplOpenGL3_CreateDeviceObjects();
    }
#endif    

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(g_Window);
    ImGui::NewFrame();


#ifdef HAVE_FREETYPETEST
    if (show_freetype)
        freetype_test.ShowFontsOptionsWindow(&show_freetype);
#endif

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


static char textbuf[1024*4];
static int tpos = 0;
static int tmax = sizeof(textbuf)-1;
static bool textChanged = false;

void addText(const char* s, int l)
{
    if (!l) return;
    
    int p = tpos + l;
    if (p > tmax)
    {
        int r = p - tmax;
        memmove(textbuf, textbuf + r, tpos - r);
        tpos -= r;
    }
    memcpy(textbuf + tpos, s, l);
    tpos += l;
    textbuf[tpos] = 0;
    textChanged = true;
}

void addText(const char* s)
{
    addText(s, strlen(s));
}

static StrandCtx sctx;

const char* gui_input_pump()
{
    // called from strand to poll for input
    const char* s = sctx.yieldCmd();
    if (s) return s; // something to do
    else
    {
        // yield back to main
        G.fibers[0].swap(&G.main);
    }
    return 0;
}

static void textReceiver(const char* s)
{
    addText(s);
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
    /*
      else if (data->EventFlag == ImGuiInputTextFlags_CallbackEdit)
      {
      int l = data->BufTextLen;
      if (l > 0)
      {
      addText("\n");
      addText(data->Buf, l);
      }
      }
    */
    return 0;
}

static void setF(const char* name)
{
    // https://github.com/ocornut/imgui/issues/1712
    ImGui::SetFocusID(ImGui::GetID(name), ImGui::GetCurrentWindow()); 
    //ImGuiContext& g = *ImGui::GetCurrentContext();
    //g.NavDisableHighlight = false;
}

void StrandWindow(bool* strand_open)
{
    ImGuiWindowFlags window_flags = 0;
    //if (no_titlebar)        window_flags |= ImGuiWindowFlags_NoTitleBar;
    //if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
    window_flags |= ImGuiWindowFlags_MenuBar;
    //window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoCollapse;
    //if (no_nav)             window_flags |= ImGuiWindowFlags_NoNav;

    //const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    //ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);

    ImGuiIO& io = ImGui::GetIO();
    //io.KeyRepeatDelay = 2.0f;

    ImGui::SetNextWindowSize(ImVec2(800, 720), ImGuiCond_FirstUseEver);
    
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScrollbarSize = 18.0f;
    style.ChildRounding = 4.0f;
    
    ImVec2 pad = style.FramePadding;
    ImVec2 spacing = style.ItemSpacing;

    // dont pass in show flag
    ImGui::Begin("Strand", 0, window_flags);

    //bogus(); return;
    
#if 0
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
            if (ImGui::MenuItem("Save", "Ctrl+S"))   { /* Do stuff */ }
            if (ImGui::MenuItem("Close", "Ctrl+W"))  { *strand_open = false; }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
#endif

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
        ImGui::TextWrapped("%s", textbuf);
        if (textChanged)
        {
            float ym = ImGui::GetScrollMaxY() + 1000; // XXX
            ImGui::SetScrollY(ym);
            textChanged = false;
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
                sctx.sendCmd(buf);                    
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
        if (ImGui::InputTextWithHint("", "type here", buf1, sizeof(buf1),
                                     tf, InputCallback))
        {
            sctx.sendCmd(buf1);
            *buf1 = 0; // clear
            reclaim_focus = true;
        }
        if (reclaim_focus)
            ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget
        
        ImGui::EndChild();
    }
    
    ImGui::End();
}

