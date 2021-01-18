#include "screen.h"
#include "emulator.h"

bool Screen::Init(Emulator& emulator){
    _emulator = &emulator;

    if(SDL_Init(SDL_INIT_VIDEO) != 0){
        std::cout << "Failed to initialize SDL. SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_WindowFlags window_flags = (SDL_WindowFlags)(
        SDL_WINDOW_OPENGL |
        SDL_WINDOW_RESIZABLE |
        SDL_WINDOW_ALLOW_HIGHDPI
    );

    _sdlWindow = SDL_CreateWindow(
        WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        window_flags
    );

    if(_sdlWindow == nullptr){
        std::cout << "Failed to initialize SDL Window. SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    _sdlContext = SDL_GL_CreateContext(_sdlWindow);
    SDL_GL_MakeCurrent(_sdlWindow, _sdlContext);
    SDL_SetWindowMinimumSize(_sdlWindow, WINDOW_WIDTH, WINDOW_HEIGHT);
    SDL_GL_SetSwapInterval(1);

    /*
        THIS NEEEDDDS TO BE AFTER MakeCurrent AND BEFORE OTHER OPENGL CALLS
    */
    if(!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)){
        std::cout << "Failed to initialize GLAD!" << std::endl;
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0); 
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForOpenGL(_sdlWindow, _sdlContext);
    ImGui_ImplOpenGL3_Init("#version 130");

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.DeltaTime = 1.0f / 60.f;

    return true;
}

void Screen::BeginRender(){
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(_sdlWindow);
    ImGui::NewFrame();

    DrawGameMenu();
    DrawMainMenu();

     if(_showAboutMenu)
       DrawAboutMenu();
}

void Screen::DrawGameMenu(){
    ImGui::SetNextWindowPos(ImVec2(0, SCREEN_START_Y));
    ImGui::SetNextWindowSize(ImVec2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2));
    ImGui::Begin("Game View");

    ImGui::End();
}

void Screen::DrawMainMenu(){
    ImGui::BeginMainMenuBar();

    if(ImGui::BeginMenu("File")){
        if(ImGui::BeginMenu("Load ROM")){
            if(ImGui::MenuItem("NES Test"))
                Emulator::GetNES()->Start("../roms/tests/nestest.nes");
            ImGui::EndMenu();
        }

        if(ImGui::MenuItem("Exit")){
            Emulator::Exit();
        }

        ImGui::EndMenu();
    }

    if(ImGui::BeginMenu("Settings")){
        if(_emulator->settings.ShowDebugView()){
            if(ImGui::MenuItem("Hide debug view")){
                _emulator->settings.SetShowDebugView(false);
            }
        }else{
            if(ImGui::MenuItem("Show debug view")){
                _emulator->settings.SetShowDebugView(true);
            }
        }
        ImGui::EndMenu();
    }

    bool cartLoaded = Emulator::GetNES()->GetBus()->IsCartridgeLoaded();

    if(cartLoaded){
        if(ImGui::MenuItem("Reset")){
            Emulator::GetNES()->Restart();
        }
    }
    
    if(ImGui::MenuItem("Pause/Unpause"))
        Emulator::GetNES()->Pause();

    if(cartLoaded){
        if(ImGui::MenuItem("Step"))
            Emulator::GetNES()->Step();
    }

    if(ImGui::MenuItem("About"))
        _showAboutMenu = !_showAboutMenu;

    ImGui::EndMainMenuBar();
}

void Screen::DrawAboutMenu(){
    ImGui::SetNextWindowPos(ImVec2(WINDOW_WIDTH / 2 - 200, WINDOW_HEIGHT / 2 - 40));
    ImGui::SetNextWindowSize(ImVec2(400, 80));

    ImGui::Begin("About");

    ImGui::Text("A NES emulator written in C++ using SDL2 and ImGui.\n\nWritten by Brendan Wilby");

    ImGui::End();
}

void Screen::EndRender(){
    ImGui::Render();
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(_sdlWindow);
}

void Screen::Destroy(){
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(_sdlContext);
    SDL_DestroyWindow(_sdlWindow);
    SDL_Quit();
}