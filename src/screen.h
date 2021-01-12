#pragma once

constexpr auto WINDOW_WIDTH = 800;
constexpr auto WINDOW_HEIGHT = 600;
constexpr auto WINDOW_TITLE = "NES Emulator";

class Screen{
    private:
        bool _showAboutMenu;
        
        SDL_Window* _sdlWindow;
        SDL_GLContext _sdlContext;
    public:
        Screen() : _sdlWindow(nullptr) {}

        bool Init();
        void BeginRender();
        void EndRender();
        void Destroy();
};