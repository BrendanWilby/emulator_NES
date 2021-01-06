constexpr auto WINDOW_WIDTH = 800;
constexpr auto WINDOW_HEIGHT = 600;
constexpr auto WINDOW_TITLE = "NES Emulator";

class Screen{
    private:
        SDL_Window* _sdlWindow;
    public:
        Screen() : _sdlWindow(nullptr) {}

        bool Init();
        void BeginRender();
        void EndRender();
        void Destroy();
};