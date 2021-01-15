#include "ppu.h"
#include "bus.h"

void PPU::Reset(){
    
}

void PPU::ConnectToBus(Bus &bus){
    _bus = &bus;
}