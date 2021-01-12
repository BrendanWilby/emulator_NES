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
}

void NES::Start(bool autoStart){
    Debugger::LogMessage("Starting boot sequence");

    Debugger::LogMessage("Resetting bus");
    _bus->Reset();

    Debugger::LogMessage("Resetting CPU");
    _cpu->Reset();

    if(autoStart)
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
    }
}

void NES::Update(){
    if(_currentState == NESState::NES_STATE_RUNNING){
         uint8_t cycles = _cpu->Execute();
    }
}

void NES::Step(){
    if(_currentState == NESState::NES_STATE_STOPPED)
        Start(false);
    else if(_currentState == NESState::NES_STATE_RUNNING)
        Pause();
    
    _currentState = NESState::NES_STATE_STEPPING;

    Debugger::LogMessage("Stepping");

    uint8_t cycles = _cpu->Execute();
}

const char* NES::GetCurrentState(){
    const char* state = "";

    switch(_currentState){
        case NESState::NES_STATE_PAUSED:
            state = "paused";
        break;

        case NESState::NES_STATE_RUNNING:
            state = "running";
        break;

        case NESState::NES_STATE_STEPPING:
            state = "stepping";
        break;

        case NESState::NES_STATE_STOPPED:
            state = "stopped";
        break;
    }

    return state;
}