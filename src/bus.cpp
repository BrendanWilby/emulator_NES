#include "bus.h"
#include "cpu.h"
#include "ppu.h"
#include "debugger.h"

void Bus::Reset(){
    for(int i = 0; i < MEM_SIZE; i++)
        Write(i, 0);
}

void Bus::ConnectCPU(CPU& cpu){
	_cpu = &cpu;
}

void Bus::ConnectPPU(PPU& ppu){
	_ppu = &ppu;
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

bool Bus::LoadROM(const char* path){
	// Open file
	std::ifstream romFile(path, std::ios::binary);

	if(!romFile){
		Debugger::LogError(std::string("Could not open file ") + std::string(path));
		return false;
	}
	
	std::vector<unsigned char> romBuffer(std::istreambuf_iterator<char>(romFile), {});
	auto romSize = romBuffer.size();

	if (romSize > 0x4000)
		romSize = 0x4000;

	// Read first 3 bytes of .nes file
	std::string fileType;
	fileType.push_back(romBuffer[0]);
	fileType.push_back(romBuffer[1]);
	fileType.push_back(romBuffer[2]);

	// Check if the first three bytes contain "NES" and the fourth byte contains 0x1A
	if(fileType != "NES" || romBuffer[3] != 0x1A){
		Debugger::LogError("File " + std::string(path) + " not recognised as a NES file.");
		return false;
	}

	_numRomBanks = romBuffer[4];
	_numVRomBanks = romBuffer[5];

	_mirrorType = (romBuffer[6] & 0x01) == 0 ? MirroringType::MIRROR_HORIZONTAL : MirroringType::MIRROR_VERTICAL;
	_hasBatteryPackedRAM = (romBuffer[6] & 0x02) >> 1;
	_hasTrainer = (romBuffer[6] & 0x04) >> 2;

	if(((romBuffer[6] & 0x08) >> 3) == 0x01)
		_mirrorType = MirroringType::MIRROR_FOUR;

	_mapperNumber = (romBuffer[7] & 0xF0) | (romBuffer[6] & 0xF0) >> 4;
	_numRamBanks = romBuffer[8];

	for (int i = 16; i < romSize; i++) {
		Write(PRG_ROM_BANK_0_START + i - 16, (uint8_t)romBuffer[i]);
		Write(PRG_ROM_BANK_1_START + i - 16, (uint8_t)romBuffer[i]);
	}

	_currentCartridge->romPath = path;
	_currentCartridge->size = romSize;

	Debugger::LogMessage(std::string("Loaded rom file: ") + std::string(path));

	_cartLoaded = true;

	return true;
}