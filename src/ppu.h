#pragma once

constexpr auto NES_SCREEN_WIDTH = 256;
constexpr auto NES_SCREEN_HEIGHT = 240;
constexpr auto PPU_VRAM_SIZE = 16 * 1024;
constexpr auto PPU_SPR_RAM_SIZE = 256;

class Bus;

class PPU {
    private:
        Bus* _bus;

        uint8_t _pixelBuffer[NES_SCREEN_WIDTH * NES_SCREEN_HEIGHT];
        uint8_t _vram[PPU_VRAM_SIZE];
        uint8_t _sprRamSize[PPU_SPR_RAM_SIZE];
    public:
        PPU(){}

        void Reset();
        void ConnectToBus(Bus &bus);
        uint8_t *GetPixelBuffer() { return _pixelBuffer; }
};