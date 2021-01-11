#pragma once

// Main RAM
constexpr auto RAM_SIZE = 2048;
constexpr auto RAM_START = 0x0000;
constexpr auto MEM_SIZE = 65536;

// Program ROM
constexpr auto PRG_ROM_SIZE = 32768;
constexpr auto PRG_ROM_BANK_0_START = 0x8000;
constexpr auto PRG_ROM_BANK_1_START = 0xC000;
constexpr auto PRG_ROM_END = 0xFFF;

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

class Bus {
    private:
        uint8_t _ram[RAM_SIZE];
        uint8_t _prgRom[PRG_ROM_SIZE];

        CPU* _cpu;
    public:
        Bus() :
            _ram(),
            _prgRom()
        {}

        void Reset();

        void ConnectCPU(CPU& cpu);

        void Write(uint16_t address, uint8_t value);

        uint8_t Read(uint16_t address);
        uint16_t Read16(uint16_t address);
};