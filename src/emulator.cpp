#include "emulator.h"

bool Emulator::_isRunning = false;
std::unique_ptr<NES> Emulator::_nes = std::make_unique<NES>();

Emulator::Emulator(){
    _isRunning = false;
    _screen = std::make_unique<Screen>();
    _input = std::make_unique<Input>();
    _debugger = std::make_unique<Debugger>(*_nes->GetCPU(), *_nes->GetBus());

    if(_screen->Init() == false){
        std::cerr << "Failed to initialize emulator" << std::endl;
        return;
    }
}

void Emulator::Start(){
    Run();
}

void Emulator::Run(){
    _isRunning = true;

    while(_isRunning){
        Uint64 start = SDL_GetPerformanceCounter();

        _input->HandleInput();

        _nes->Update();

        _screen->BeginRender();

        // Rendering here
        _debugger->Render();

        _screen->EndRender();

        // Frame rate cap
        Uint64 end = SDL_GetPerformanceCounter();

        float elapsed = (end - start) / (float)SDL_GetPerformanceFrequency() * 1000.0f;

        if (elapsed < 1000.0f / 60.0f)
            SDL_Delay((1000.0f / 60.0f) - elapsed);
    }

    OnQuit();
}

void Emulator::OnQuit(){
    _screen->Destroy();
    std::cout << "Quit Successfully" << std::endl;
}

void Emulator::Exit(){
    _isRunning = false;
}