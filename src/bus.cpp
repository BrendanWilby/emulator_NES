#include "bus.h"
#include "cpu.h"

void Bus::Reset(){
    for(int i = 0; i < RAM_SIZE; i++)
        _ram[i] = 0;
	for(int i = 0; i < PRG_ROM_SIZE; i++)
		_prgRom[i] = 0;
}

void Bus::ConnectCPU(CPU& cpu){
	_cpu = &cpu;
}

void Bus::Write(uint16_t address, uint8_t value){
    if (address >= PRG_ROM_BANK_0_START && address < PRG_ROM_END) {
		_prgRom[address - PRG_ROM_BANK_0_START] = value;
	}else if (address >= RAM_START && address <= RAM_SIZE) {
		_ram[address & 0x07FF] = value;
	} else if (address >= IO_PPU_START && address <= IO_PPU_END) {
		_ram[address & 0x07FF] = value;
		//_ppu->WriteRegisters(address & 0x0007, value);
	} else if (address >= IO_GEN_START && address <= IO_GEN_END) {
		_ram[address & 0x07FF] = value;
	}
}

uint8_t Bus::Read(uint16_t address){
    uint8_t data = 0;

    if (address >= PRG_ROM_BANK_0_START && address < PRG_ROM_END) {
		data = _prgRom[address - PRG_ROM_BANK_0_START];
		// Mirror every 2KB between 0x0000 -> 0x1FFF
	} else if (address >= RAM_START && address <= RAM_SIZE)
		data = _ram[address & 0x07FF];
	// Mirror every 8 bytes between 0x2000 -> 0x3FFF
	else if (address >= IO_PPU_START && address <= IO_PPU_END)
		data = _ram[address & 0x0007];
	else if (address <= IO_GEN_START && address <= IO_GEN_END)
		data = _ram[address & 0x07FF];

	return data;
}

uint16_t Bus::Read16(uint16_t address){
    return (Read(address + 0x0001) << 8) | Read(address);
}