#include "nes.h"
#include "cpu.h"
#include "bus.h"

NES::NES(){
    _bus = std::make_unique<Bus>();
    _cpu = std::make_unique<CPU>();

    _bus->ConnectCPU(*_cpu);
    _cpu->ConnectToBus(*_bus);
}

void NES::Start(){
    std::cout << "Starting boot sequence..." << std::endl;

    std::cout << "Resetting bus" << std::endl;
    _bus->Reset();

    std::cout << "Resetting CPU" << std::endl;
    _cpu->Reset();
}

void NES::Update(){
    uint8_t cycles = _cpu->Execute();
}