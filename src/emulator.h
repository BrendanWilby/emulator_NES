#pragma once

#include "screen.h"
#include "nes.h"
#include "input.h"
#include "nes.h"
#include "cpu.h"
#include "bus.h"
#include "debugger.h"

class Emulator{
    private:
        static bool _isRunning;

        std::unique_ptr<Screen> _screen;
        std::unique_ptr<Input> _input;
        std::unique_ptr<NES> _nes;
        std::unique_ptr<Debugger> _debugger;
        
    public:
        Emulator();
        
        void Start();
        void Run();
        void Quit();

        static void Exit();
};