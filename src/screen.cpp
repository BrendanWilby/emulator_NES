#include "screen.h"

bool Screen::Init(){
    if(SDL_Init(SDL_INIT_VIDEO) != 0){
        std::cout << "Failed to initialize SDL. SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_WindowFlags window_flags = (SDL_WindowFlags)(
        SDL_WINDOW_OPENGL
        | SDL_WINDOW_RESIZABLE
        | SDL_WINDOW_ALLOW_HIGHDPI
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

    SDL_SetWindowMinimumSize(_sdlWindow, WINDOW_WIDTH, WINDOW_HEIGHT);
    SDL_GLContext context = SDL_GL_CreateContext(_sdlWindow);
    SDL_GL_MakeCurrent(_sdlWindow, context);

    /*
        THIS NEEEDDDS TO BE AFTER MakeCurrent AND BEFORE OTHER OPENGL CALLS
    */
    if(!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)){
        std::cout << "Failed to initialize GLAD!" << std::endl;
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_GL_SetAttribute(
        SDL_GL_CONTEXT_PROFILE_MASK,
        SDL_GL_CONTEXT_PROFILE_CORE
    );

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0); 
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    SDL_GL_SetSwapInterval(1);

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForOpenGL(_sdlWindow, context);
    ImGui_ImplOpenGL3_Init();

    return true;
}

void Screen::BeginRender(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(_sdlWindow);
    ImGui::NewFrame();

    if (ImGui::BeginMainMenuBar()){

        if(ImGui::BeginMenu("File")){
            if(ImGui::MenuItem("New")){
                std::cout << "Hello" << std::endl;
            }
            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("About")){

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void Screen::EndRender(){
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(_sdlWindow);
}

void Screen::Destroy(){
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_DestroyWindow(_sdlWindow);
}