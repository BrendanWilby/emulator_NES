#pragma once

#include "screen.h"
#include "nes.h"
#include "input.h"
#include "nes.h"
#include "cpu.h"
#include "ppu.h"
#include "bus.h"
#include "debugger.h"

class EmulatorSettings {
    private:
        bool _showDebugView;

    public:
        EmulatorSettings(){}

        bool ShowDebugView(){ return _showDebugView; }
        void SetShowDebugView(bool showDebug) { _showDebugView = showDebug; }
};

class Emulator {
    private:
        static bool _isRunning;

        std::unique_ptr<Screen> _screen;
        std::unique_ptr<Input> _input;
        static std::unique_ptr<NES> _nes;
        std::unique_ptr<Debugger> _debugger;
    public:
        EmulatorSettings settings;
    public:
        Emulator();
        
        void Start();
        void Run();
        void OnQuit();

        static void Exit();


        static NES* GetNES(){ return _nes.get(); }
};