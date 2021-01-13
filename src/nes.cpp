#include "nes.h"
#include "cpu.h"
#include "bus.h"
#include "debugger.h"

NES::NES(){
    _bus = std::make_unique<Bus>();
    _cpu = std::make_unique<CPU>();

    _bus->ConnectCPU(*_cpu);
    _cpu->ConnectToBus(*_bus);
    _currentState = NESState::NES_STATE_STOPPED;
    
    _bus->Reset();
    _cpu->Reset();
}

void NES::Restart(){
    if(_bus->IsCartridgeLoaded())
        Start(_bus->GetCurrentCartPath());
}

void NES::Start(const char* romPath){
    Debugger::LogMessage("Starting boot sequence");

    Debugger::LogMessage("Resetting bus");
    _bus->Reset();

    Debugger::LogMessage("Resetting CPU");
    _cpu->Reset();

    // Load BIOS here at some point

    _bus->LoadCartridge(romPath);

    if(_currentState != NESState::NES_STATE_PAUSED)
        _currentState = NESState::NES_STATE_RUNNING;
}

void NES::Pause(){
    if(_currentState == NESState::NES_STATE_RUNNING){
        _currentState = NESState::NES_STATE_PAUSED;
        Debugger::LogMessage("Paused");
    }else if(_currentState == NESState::NES_STATE_PAUSED){
        _currentState = NESState::NES_STATE_RUNNING;
        Debugger::LogMessage("Unpaused");
    }else if(_currentState == NESState::NES_STATE_STEPPING){
        _currentState = NESState::NES_STATE_RUNNING;
        Debugger::LogMessage("Continuing automatic execution");
    }else if(_currentState == NESState::NES_STATE_STOPPED){
        _currentState = NESState::NES_STATE_PAUSED;
        Debugger::LogMessage("Paused on start");
    }
}

void NES::Update(){
    if(_currentState == NESState::NES_STATE_RUNNING){
         uint8_t cycles = _cpu->Execute();
    }
}

void NES::Step(){
    _currentState = NESState::NES_STATE_STEPPING;

    Debugger::LogMessage("Stepping");

    uint8_t cycles = _cpu->Execute();
}

const char* NES::GetCurrentState(){
    const char* state = "";

    switch(_currentState){
        case NESState::NES_STATE_PAUSED:
            state = "Paused";
        break;

        case NESState::NES_STATE_RUNNING:
            state = "Running";
        break;

        case NESState::NES_STATE_STEPPING:
            state = "Stepping";
        break;

        case NESState::NES_STATE_STOPPED:
            state = "Stopped";
        break;
    }

    return state;
}