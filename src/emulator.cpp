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
        _isRunning = _input->HandleInput();
        _screen->BeginRender();

        // Rendering here

        _screen->EndRender();
    }

    Quit();
}

void Emulator::Quit(){
    _screen->Destroy();
    _nes->Destroy();
    std::cout << "Quit Successfully" << std::endl;
}