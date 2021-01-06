#pragma once
#include "screen.h"
#include "nes.h"
#include "input.h"

class Emulator{
    private:
        bool _isRunning;

        std::unique_ptr<Screen> _screen;
        std::unique_ptr<Input> _input;
        std::unique_ptr<NES> _nes;
        
    public:
        Emulator();
        
        void Start();
        void Run();
        void Quit();
};