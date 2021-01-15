#pragma once

// Main RAM
constexpr auto RAM_SIZE = 2048;
constexpr auto RAM_START = 0x0000;
constexpr auto MEM_SIZE = 65536;

// Program ROM
constexpr auto PRG_ROM_SIZE = 32768;
constexpr auto PRG_ROM_BANK_0_START = 0x8000;
constexpr auto PRG_ROM_BANK_1_START = 0xC000;
constexpr auto PRG_ROM_END = 0xFFFF;

// IO
constexpr auto IO_PPU_START = 0x2000;
constexpr auto IO_PPU_END = 0x3FFF;
constexpr auto IO_GEN_START = 0x3FFF;
constexpr auto IO_GEN_END = 0x4019;

constexpr auto STACK_START = 0x0100;
constexpr auto IRQ_VECTOR_START = 0xFFFE;
constexpr auto RESET_VECTOR_START = 0xFFFC;
constexpr auto RESET_VECTOR_LOW = 0x00;
constexpr auto RESET_VECTOR_HIGH = 0x80;

class CPU;
class PPU;

struct Cartridge {
    const char* path;
    uint16_t size;
};

class Bus {
    private:
        uint8_t _ram[RAM_SIZE];
        uint8_t _prgRom[PRG_ROM_SIZE];

        CPU* _cpu;
        PPU* _ppu;
        std::unique_ptr<Cartridge> _currentCartridge;
        bool _cartLoaded;
    public:
        Bus() :
            _ram(),
            _prgRom(),
            _currentCartridge(std::make_unique<Cartridge>()),
            _cartLoaded(false)
        {}

        void Reset();

        void ConnectCPU(CPU& cpu);
        void ConnectPPU(PPU& ppu);

        void Write(uint16_t address, uint8_t value);

        bool LoadCartridge(const char* path);
        bool IsCartridgeLoaded() { return _cartLoaded; };
        const char* GetCurrentCartPath(){ return _currentCartridge->path; };

        uint8_t Read(uint16_t address);
        uint16_t Read16(uint16_t address);

        uint8_t *GetRAM(){ return _ram; }
        uint8_t *GetROM(){ return _prgRom; }
};