#pragma once

constexpr auto WINDOW_WIDTH = 800;
constexpr auto WINDOW_HEIGHT = 600;
constexpr auto WINDOW_TITLE = "NES Emulator";

constexpr auto MENU_MAIN_HEIGHT = 20;
constexpr auto SCREEN_WIDTH = 800;
constexpr auto SCREEN_HEIGHT = 600 - MENU_MAIN_HEIGHT;
constexpr auto SCREEN_START_X = 0;
constexpr auto SCREEN_START_Y = MENU_MAIN_HEIGHT;

class Screen {
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

        void DrawGameMenu();
        void DrawMainMenu();
        void DrawAboutMenu();

        SDL_Window* GetSDLWindow(){ return _sdlWindow; }
};