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
}

void NES::Start(){
    Debugger::LogMessage("Starting boot sequence...");

    Debugger::LogMessage("Resetting bus...");
    _bus->Reset();

    Debugger::LogMessage("Resetting CPU...");
    _cpu->Reset();

    _paused = false;
}

void NES::Pause(){
    _paused = !_paused;

    if(_paused)
        Debugger::LogMessage("Paused");
    else
        Debugger::LogMessage("Unpaused");
}

void NES::Update(){
    if(_paused == false){
        uint8_t cycles = _cpu->Execute();
    }
}