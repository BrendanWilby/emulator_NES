#include "emulator.h"

Emulator::Emulator(){
    _isRunning = false;
    _screen = std::make_unique<Screen>();
    _input = std::make_unique<Input>();
    _nes = std::make_unique<NES>();

    if(_screen->Init() == false){
        std::cout << "Failed to initialize emulator" << std::endl;
        return;
    }
}

void Emulator::Start(){
    _nes->Start();

    Run();
}

void Emulator::Run(){
    _isRunning = true;

    while(_isRunning){
        Uint64 start = SDL_GetPerformanceCounter();

        _isRunning = _input->HandleInput();
        _screen->BeginRender();

        // Rendering here

        _screen->EndRender();

        // Frame rate cap

        Uint64 end = SDL_GetPerformanceCounter();

        float elapsed = (end - start) / (float)SDL_GetPerformanceFrequency() * 1000.0f;

        if (elapsed < 1000.0f / 60.0f)
            SDL_Delay((1000.0f / 60.0f) - elapsed);
    }

    Quit();
}

void Emulator::Quit(){
    _screen->Destroy();
    _nes->Destroy();
    std::cout << "Quit Successfully" << std::endl;
}