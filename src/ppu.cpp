#include "ppu.h"
#include "bus.h"
#include "debugger.h"

void PPU::Reset(){
    for(int i = 0; i < NES_SCREEN_WIDTH * NES_SCREEN_HEIGHT; i++)
        _pixelBuffer[i] = 0;


    for(int i = 0; i < 1024; i++)
        _pixelBuffer[i] = 240;
}

void PPU::ConnectToBus(Bus &bus){
    _bus = &bus;
}