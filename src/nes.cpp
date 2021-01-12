#include "nes.h"
#include "cpu.h"
#include "bus.h"
#include "debugger.h"

NES::NES(){
    _bus = std::make_unique<Bus>();
    _cpu = std::make_unique<CPU>();

    _bus->ConnectCPU(*_cpu);
    _cpu->ConnectToBus(*_bus);
    _paused = true;
    _running = false;
}

void NES::Start(){
    Debugger::LogMessage("Starting boot sequence...");

    Debugger::LogMessage("Resetting bus...");
    _bus->Reset();

    Debugger::LogMessage("Resetting CPU...");
    _cpu->Reset();

    _paused = false;
    _running = true;
}

void NES::Pause(){
    if(_running){
        _paused = !_paused;

        if(_paused)
            Debugger::LogMessage("Paused");
        else
            Debugger::LogMessage("Unpaused");
    }
}

void NES::Update(){
    if(_paused == false){
        uint8_t cycles = _cpu->Execute();
    }
}