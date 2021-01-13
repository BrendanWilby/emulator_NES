#include "cpu.h"
#include "bus.h"

uint8_t CPU::Execute() {
	// Fetch opcode
	uint8_t opCode = _bus->Read(_pc);

	_lastPC = _pc;
	_currentOpCode = opCode;
	_currentOpMnemonic = _instructions[opCode].mnemonic;

	// Decode and execute the instruction
	(this->*(_instructions[opCode].execute))(opCode);

	// Return cycles
	return _instructions[opCode].cycles + _addCycles;
}

void CPU::Reset() {
	_pc = 0xC000;
	_lastPC = 0xC000;
	_currentOpCode = 0;
	_currentOpMnemonic = "";
	_sp = 0xFD;
	_regA = 0;
	_regX = 0;
	_regY = 0;
	_flags = 0x24;
	_addCycles = 0;
}

void CPU::ConnectToBus(Bus& bus) {
	_bus = &bus;
}

/*
	============================================
	HELPER FUNCTIONS
	============================================
*/

void CPU::SetFlag(Flags flag, FlagStatus status) {
	uint8_t flagValue = static_cast<uint8_t>(flag);
	_flags = (status == FLAG_SET ? (_flags | flagValue) : (_flags & ~flagValue));
}

FlagStatus CPU::GetFlag(Flags flag) {
	return (_flags & static_cast<uint8_t>(flag)) != 0;
}

// Stack pointer implemented backwards, so top of stack is at 0x1FF
// _sp then starts at 0xFF and decrements
void CPU::PushStack8(uint8_t value) {
	_bus->Write(STACK_START + static_cast<uint16_t>(--_sp), value);
}

void CPU::PushStack16(uint16_t value) {
	PushStack8(static_cast<uint8_t>(value & 0x00FF));
	PushStack8(static_cast<uint8_t>((value & 0xFF00) >> 8));
}

uint8_t CPU::PopStack8() {
	return _bus->Read(STACK_START + static_cast<uint16_t>(_sp++));
}

uint16_t CPU::PopStack16() {
	uint8_t high = PopStack8();
	uint8_t low = PopStack8();
	return (high << 8) | low;
}

bool CPU::CheckPageChange(uint16_t addrOld, uint16_t addrNew) {
	uint8_t oldPage = static_cast<uint8_t>((addrOld & 0xFF00) >> 8);
	uint8_t newPage = static_cast<uint8_t>((addrNew & 0xFF00) >> 8);

	if (newPage - oldPage >= 0x01)
		return true;
	return false;
}

/*
	============================================
	ADDRESSING MODES
	============================================
*/

uint16_t CPU::ReadAddrMode_ABS() {
	uint8_t high = _bus->Read(_pc + 2);
	uint8_t low = _bus->Read(_pc + 1);
	return (high << 8) | low;
}

uint16_t CPU::ReadAddrMode_ABS_X(bool checkPage) {
	uint8_t high = _bus->Read(_pc + 2);
	uint8_t low = _bus->Read(_pc + 1);
	uint16_t addr = (high << 8) | low;
	uint16_t tAddr = addr + static_cast<uint16_t>(_regX);

	if (checkPage && CheckPageChange(addr, tAddr))
		_addCycles = 1;

	return tAddr;
}

uint16_t CPU::ReadAddrMode_ABS_Y(bool checkPage) {
	uint8_t high = _bus->Read(_pc + 2);
	uint8_t low = _bus->Read(_pc + 1);
	uint16_t addr = (high << 8) | low;

	uint16_t tAddr = addr + static_cast<uint16_t>(_regY);

	if (checkPage && CheckPageChange(addr, tAddr))
		_addCycles = 1;

	return tAddr;
}

uint16_t CPU::ReadAddrMode_IMM() {
	return _pc + 1;
}

uint16_t CPU::ReadAddrMode_IND() {
	uint16_t addr = (uint16_t)_bus->Read(_pc + 1);

	uint8_t tAddrLow = _bus->Read(addr);
	uint8_t tAddrHigh = _bus->Read(addr + 1);

	return (tAddrHigh << 8 | tAddrLow);
}

uint16_t CPU::ReadAddrMode_X_IND() {
	uint8_t operand = _bus->Read(_pc + 1);
	uint16_t addr = static_cast<uint16_t>(operand) + static_cast<uint16_t>(_regX);

	uint8_t tAddrLow = _bus->Read(addr);
	uint8_t tAddrHigh = _bus->Read(addr + 1);

	return (tAddrHigh << 8 | tAddrLow);
}

uint16_t CPU::ReadAddrMode_IND_Y(bool checkPage) {
	uint16_t addr = static_cast<uint16_t>(_bus->Read(_pc + 1));
	uint8_t tAddrLow = _bus->Read(addr);

	uint16_t tAddr = static_cast<uint16_t>(tAddrLow) + static_cast<uint16_t>(_regY);

	if (checkPage && CheckPageChange(tAddrLow, tAddr))
		_addCycles = 1;

	return tAddr;
}

uint16_t CPU::ReadAddrMode_REL(bool checkPage) {
	int8_t offset = (int8_t)_bus->Read(_pc + 1);
	_addCycles = 1;
	uint16_t newPC = _pc + static_cast<uint16_t>(offset);

	if (checkPage && CheckPageChange(_pc, newPC))
		_addCycles = 3;

	return newPC;
}

uint16_t CPU::ReadAddrMode_ZPG() {
	return (uint16_t)_bus->Read(_pc + 1);
}

uint16_t CPU::ReadAddrMode_ZPG_X() {
	uint16_t addr = (uint16_t)_bus->Read(_pc + 1);
	return addr + (uint16_t)_regX;
}

uint16_t CPU::ReadAddrMode_ZPG_Y() {
	uint16_t addr = (uint16_t)_bus->Read(_pc + 1);
	return addr + (uint16_t)_regY;
}

#include "cpu.h"
#include "bus.h"

void CPU::BRK(uint8_t opCode) {
	// push PC onto stack
	PushStack16(_pc);

	// push status onto stack
	PushStack8(_flags);

	// address at 0xFFFF, 0xFFFE is loaded into the PC
	_pc = _bus->Read16(IRQ_VECTOR_START);

	// Break flag set
	SetFlag(CPU::Flags::FLAG_B, FLAG_SET);
}

void CPU::ORA(uint8_t opCode) {
	uint16_t addr = 0;

	if (opCode == 0x01) {
		addr = ReadAddrMode_X_IND();
		_pc += 2;
	}else if (opCode == 0x05) {
		addr = ReadAddrMode_ZPG();
		_pc += 2;
	}else if (opCode == 0x09) {
		addr = ReadAddrMode_IMM();
		_pc += 2;
	}else if (opCode == 0x0D) {
		addr = ReadAddrMode_ABS();
		_pc += 3;
	}else if (opCode == 0x11) {
		uint16_t oldRegY = _regY;
		addr = ReadAddrMode_IND_Y(true);
		_pc += 2;
	}else if (opCode == 0x15) {
		addr = ReadAddrMode_ZPG_X();
		_pc += 2;
	}else if (opCode == 0x19) {
		uint16_t oldRegY = _regY;
		addr = ReadAddrMode_ABS_Y(true);
		_pc += 3;
	}else if (opCode == 0x1D) {
		uint16_t oldRegX = _regX;
		addr = ReadAddrMode_ABS_X(true);
		_pc += 3;
	}

	_regA |= _bus->Read(addr);
	SetFlag(Flags::FLAG_Z, _regA == 0);
	SetFlag(Flags::FLAG_N, (_regA & 0x80) != 0);
}

void CPU::ASL(uint8_t opCode) {
	uint16_t addr = 0;
	uint8_t val = 0;

	if (opCode == 0x06) {
		addr = ReadAddrMode_ZPG();
		val = _bus->Read(addr);
		_pc += 2;
	}else if (opCode == 0x0A) {
		val = _regA;
		_pc += 1;
	}else if (opCode == 0x0E) {
		addr = ReadAddrMode_ABS();
		val = _bus->Read(addr);
		_pc += 3;
	}else if (opCode == 0x16) {
		addr = ReadAddrMode_ZPG_X();
		val = _bus->Read(addr);
		_pc += 2;
	}else if (opCode == 0x1E) {
		addr = ReadAddrMode_ABS_X();
		val = _bus->Read(addr);
		_pc += 3;
	}

	uint8_t valShift = val << 1;
	valShift &= 0xFE;

	if (opCode == 0x0A) {
		_regA = valShift;
		SetFlag(Flags::FLAG_Z, _regA == 0);
	}else {
		_bus->Write(addr, valShift);
	}

	SetFlag(Flags::FLAG_C, (val & 0x80) != 0);
	SetFlag(Flags::FLAG_N, (valShift & 0x80) != 0);
}

void CPU::PHP(uint8_t opCode) {
	PushStack8(_flags);
	_pc += 1;
}

void CPU::BPL(uint8_t opCode) {
	if (GetFlag(Flags::FLAG_N) == FLAG_CLEAR) {
		_pc = ReadAddrMode_REL(true);
	}else {
		_pc += 2;
	}
}

void CPU::CLC(uint8_t opCode) {
	SetFlag(Flags::FLAG_C, FLAG_CLEAR);
	_pc += 1;
}

void CPU::JSR(uint8_t opCode) {
	uint16_t tAddr = ReadAddrMode_ABS();

	PushStack16(_pc + 2);

	_pc = tAddr;
}

void CPU::AND(uint8_t opCode) {
	uint16_t addr = 0;

	if (opCode == 0x21) {
		addr = ReadAddrMode_X_IND();
		_pc += 2;
	}else if (opCode == 0x25) {
		addr = ReadAddrMode_ZPG();
		_pc += 2;
	}else if (opCode == 0x29) {
		addr = ReadAddrMode_IMM();
		_pc += 2;
	}else if (opCode == 0x2D) {
		addr = ReadAddrMode_ABS();
		_pc += 3;
	}else if (opCode == 0x31) {
		addr = ReadAddrMode_IND_Y(true);
		_pc += 2;
	}else if (opCode == 0x35) {
		addr = ReadAddrMode_ZPG_X();
		_pc += 2;
	}else if (opCode == 0x39) {
		addr = ReadAddrMode_ABS_Y(true);
		_pc += 3;
	}else if (opCode == 0x3D) {
		addr = ReadAddrMode_ABS_X(true);
		_pc += 3;
	}

	uint8_t val = _bus->Read(addr);
	_regA &= val;

	SetFlag(Flags::FLAG_C, _regA == 0);
	SetFlag(Flags::FLAG_N, (_regA & 0x80) != 0);
}

void CPU::BIT(uint8_t opCode) {
	uint16_t addr = 0;

	if (opCode == 0x24) {
		addr = ReadAddrMode_ZPG();
		_pc += 2;
	}else if (opCode == 0x2C) {
		addr = ReadAddrMode_ABS();
		_pc += 3;
	}

	uint8_t val = _bus->Read(addr);
	SetFlag(Flags::FLAG_Z, (_regA & val) == 0);
	SetFlag(Flags::FLAG_V, (val & 0x40) != 0);
	SetFlag(Flags::FLAG_N, (val & 0x80) != 0);
}

void CPU::ROL(uint8_t opCode) {
	uint16_t addr = 0;
	uint8_t val = 0;

	if (opCode == 0x26) {
		addr = ReadAddrMode_ZPG();
		val = _bus->Read(addr);
		_pc += 2;
	}else if (opCode == 0x2A) {
		val = _regA;
		_pc += 1;
	}else if (opCode == 0x2E) {
		addr = ReadAddrMode_ABS();
		val = _bus->Read(addr);
		_pc += 3;
	}else if (opCode == 0x36) {
		addr = ReadAddrMode_ZPG_X();
		val = _bus->Read(addr);
		_pc += 2;
	}else if (opCode == 0x3E) {
		addr = ReadAddrMode_ABS_X();
		val = _bus->Read(addr);
		_pc += 3;
	}

	uint8_t valShift = val << 1;
	valShift |= (GetFlag(Flags::FLAG_C) == FLAG_SET ? 0x01 : 0x00);

	// Flags here
	SetFlag(Flags::FLAG_C, (val & 0x80) != 0);
	SetFlag(Flags::FLAG_N, (valShift & 0x80) != 0);

	if (opCode == 0x2A) {
		_regA = valShift;
		SetFlag(Flags::FLAG_Z, _regA == 0);
	}else {
		_bus->Write(addr, valShift);
	}
}

void CPU::PLP(uint8_t opCode) {
	_flags = PopStack8();
	_pc += 2;
}

void CPU::BMI(uint8_t opCode) {
	if (GetFlag(Flags::FLAG_N) == FLAG_SET) {
		_pc += ReadAddrMode_REL(true);
	}else {
		_pc += 2;
	}
}

void CPU::SEC(uint8_t opCode) {
	SetFlag(Flags::FLAG_C, FLAG_SET);
	_pc += 1;
}

void CPU::RTI(uint8_t opCode) {
	_flags = PopStack8();
	_pc = PopStack8();
}

void CPU::EOR(uint8_t opCode) {
	uint16_t addr = 0;

	if (opCode == 0x41) {
		addr = ReadAddrMode_X_IND();
		_pc += 2;
	}else if (opCode == 0x45) {
		addr = ReadAddrMode_ZPG();
		_pc += 2;
	}else if (opCode == 0x49) {
		addr = ReadAddrMode_IMM();
		_pc += 2;
	}else if (opCode == 0x4D) {
		addr = ReadAddrMode_ABS();
		_pc += 3;
	}else if (opCode == 0x51) {
		addr = ReadAddrMode_IND_Y(true);
		_pc += 2;
	}else if (opCode == 0x55) {
		addr = ReadAddrMode_ZPG_X();
		_pc += 2;
	}else if (opCode == 0x59) {
		addr = ReadAddrMode_ABS_Y(true);
		_pc += 3;
	}else if (opCode == 0x5D) {
		addr = ReadAddrMode_ABS_X(true);
		_pc += 3;
	}

	uint8_t val = _bus->Read(addr);
	_regA ^= val;
	SetFlag(Flags::FLAG_Z, _regA == 0);
	SetFlag(Flags::FLAG_N, (_regA & 0x80) != 0);
}

void CPU::LSR(uint8_t opCode) {
	uint16_t addr = 0;
	uint8_t val = 0;

	if (opCode == 0x46) {
		addr = ReadAddrMode_ZPG();
		_pc += 2;
	} else if (opCode == 0x4A) {
		val = _regA;
		_pc += 1;
	} else if (opCode == 0x4E) {
		addr = ReadAddrMode_ABS();
		_pc += 3;
	} else if (opCode == 0x56) {
		addr = ReadAddrMode_ZPG_X();
		_pc += 2;
	} else if (opCode == 0x5E) {
		addr = ReadAddrMode_ABS_X();
		_pc += 3;
	}

	uint8_t valShift = val >> 1;
	valShift &= 0x7F;

	SetFlag(Flags::FLAG_C, (val & 0x01) != 0);
	SetFlag(Flags::FLAG_Z, valShift == 0);
	SetFlag(Flags::FLAG_N, (valShift & 0x80) != 0);

	if (opCode == 0x4A) {
		_regA = val;
	} else {
		_bus->Write(addr, val);
	}
}

void CPU::PHA(uint8_t opCode) {
	PushStack8(_regA);
	_pc += 1;
}

void CPU::JMP(uint8_t opCode) {
	uint16_t tAddr = 0;

	if (opCode == 0x04C) {
		tAddr = ReadAddrMode_ABS();
	} else if (opCode == 0x6C) {
		uint16_t addr = ReadAddrMode_IND();

		// Should never fetch the target address if the indirect address lies on a page boundary
		if ((addr & 0x00FF) == 0x00FF)
			tAddr = (_bus->Read(addr & 0xFF00) << 8) | _bus->Read(addr);
		else 
			tAddr = (_bus->Read(addr + 1) << 8) | _bus->Read(addr);
		
	}

	_pc = tAddr;
}

void CPU::BVC(uint8_t opCode) {
	uint16_t offset = ReadAddrMode_REL(true);

	if (GetFlag(Flags::FLAG_V) == FLAG_CLEAR)
		_pc += offset;
	else
		_pc += 2;
}

void CPU::CLI(uint8_t opCode) {
	SetFlag(Flags::FLAG_I, FLAG_CLEAR);
	_pc += 1;
}

void CPU::RTS(uint8_t opCode) {
	_pc = PopStack16() + 0x0001;
}

void CPU::ADC(uint8_t opCode) {
	uint16_t addr = 0;
	uint8_t val = 0;

	if (opCode == 0x61) {
		addr = ReadAddrMode_X_IND();
		_pc += 2;
	} else if (opCode == 0x65) {
		addr = ReadAddrMode_ZPG();
		_pc += 2;
	} else if (opCode == 0x69) {
		addr = ReadAddrMode_IMM();
		_pc += 2;
	} else if (opCode == 0x6D) {
		addr = ReadAddrMode_ABS();
		_pc += 3;
	} else if (opCode == 0x71) {
		addr = ReadAddrMode_IND_Y(true);
		_pc += 2;
	} else if (opCode == 0x75) {
		addr = ReadAddrMode_ZPG_X();
		_pc += 2;
	} else if (opCode == 0x79) {
		addr = ReadAddrMode_ABS_Y(true);
		_pc += 3;
	} else if (opCode == 0x7D) {
		addr = ReadAddrMode_ABS_X(true);
		_pc += 3;
	}

	val = _bus->Read(addr);

	uint16_t result = static_cast<uint16_t>(_regA)
					+ static_cast<uint16_t>(val)
					+ static_cast<uint16_t>(GetFlag(Flags::FLAG_C) == FLAG_SET ? 1 : 0);

	_regA = static_cast<uint8_t>(result);

	SetFlag(Flags::FLAG_C, result > 0x00FF);
	SetFlag(Flags::FLAG_Z, _regA == 0);
	SetFlag(Flags::FLAG_V,
			  (~(static_cast<uint16_t>(_regA) ^ static_cast<uint16_t>(val))
			& (static_cast<uint16_t>(_regA) ^ static_cast<uint16_t>(result))
			& 0x0080)
			!= 0);
	SetFlag(Flags::FLAG_N, (_regA & 0x80) != 0);
}

void CPU::ROR(uint8_t opCode) {
	uint16_t addr = 0;
	uint8_t val = 0;

	if (opCode == 0x66) {
		addr = ReadAddrMode_ZPG();
		val = _bus->Read(addr);
		_pc += 2;
	} else if (opCode == 0x6A) {
		val = _regA;
		_pc += 1;
	} else if (opCode == 0x6E) {
		addr = ReadAddrMode_ABS();
		val = _bus->Read(addr);
		_pc += 3;
	} else if (opCode == 0x76) {
		addr = ReadAddrMode_ZPG_X();
		val = _bus->Read(addr);
		_pc += 2;
	} else if (opCode == 0x7E) {
		addr = ReadAddrMode_ABS_X();
		val = _bus->Read(addr);
		_pc += 3;
	}

	uint8_t valShift = val >> 1;
	valShift |= (GetFlag(Flags::FLAG_C) == FLAG_SET ? 0x80 : 0x00);

	SetFlag(Flags::FLAG_C, val & 0x01);
	SetFlag(Flags::FLAG_N, (val & 0x80) != 0);

	if (opCode == 0x6A) {
		_regA = valShift;
		SetFlag(Flags::FLAG_Z, _regA == 0);
	} else {
		_bus->Write(addr, valShift);
	}
}

void CPU::PLA(uint8_t opCode) {
	_regA = PopStack8();

	SetFlag(Flags::FLAG_Z, _regA == 0);
	SetFlag(Flags::FLAG_N, (_regA & 0x80) != 0);
	_pc += 1;
}

void CPU::BVS(uint8_t opCode) {
	uint16_t offset = ReadAddrMode_REL(true);

	if (GetFlag(Flags::FLAG_V) == FLAG_SET)
		_pc += offset;
	else
		_pc += 2;
}

void CPU::SEI(uint8_t opCode) {
	SetFlag(Flags::FLAG_I, FLAG_SET);
	_pc += 1;
}

void CPU::STA(uint8_t opCode) {
	uint16_t addr = 0;

	if (opCode == 0x81) {
		addr = ReadAddrMode_X_IND();
		_pc += 2;
	} else if (opCode == 0x85) {
		addr = ReadAddrMode_ZPG();
		_pc += 2;
	} else if (opCode == 0x8D) {
		addr = ReadAddrMode_ABS();
		_pc += 3;
	} else if (opCode == 0x91) {
		addr = ReadAddrMode_IND_Y();
		_pc += 2;
	} else if (opCode == 0x95) {
		addr = ReadAddrMode_ZPG_X();
		_pc += 2;
	} else if (opCode == 0x99) {
		addr = ReadAddrMode_ABS_Y();
		_pc += 3;
	} else if (opCode == 0x9D) {
		addr = ReadAddrMode_ABS_X();
		_pc += 3;
	}

	_bus->Write(addr, _regA);
}

void CPU::STY(uint8_t opCode) {
	uint16_t addr = 0;

	if (opCode == 0x84) {
		addr = ReadAddrMode_ZPG();
		_pc += 2;
	} else if (opCode == 0x8C) {
		addr = ReadAddrMode_ABS();
		_pc += 3;
	} else if (opCode == 0x94) {
		addr = ReadAddrMode_ZPG_X();
		_pc += 2;
	}

	_bus->Write(addr, _regY);
}

void CPU::STX(uint8_t opCode) {
	uint16_t addr = 0;

	if (opCode == 0x86) {
		addr = ReadAddrMode_ZPG();
		_pc += 2;
	} else if (opCode == 0x8E) {
		addr = ReadAddrMode_ABS();
		_pc += 3;
	} else if (opCode == 0x96) {
		addr = ReadAddrMode_ZPG_Y();
		_pc += 2;
	}

	_bus->Write(addr, _regX);
}

void CPU::DEY(uint8_t opCode) {
	_regY -= 1;
	SetFlag(Flags::FLAG_Z, _regY == 0);
	SetFlag(Flags::FLAG_N, (_regY & 0x80) != 0);
	_pc += 1;
}

void CPU::TXA(uint8_t opCode) {
	_regA = _regX;
	SetFlag(Flags::FLAG_Z, _regA == 0);
	SetFlag(Flags::FLAG_N, (_regA & 0x80) != 0);
	_pc += 1;
}

void CPU::BCC(uint8_t opCode) {
	if (GetFlag(Flags::FLAG_C) == FLAG_CLEAR) {
		_pc += ReadAddrMode_REL(true);
	} else {
		_pc += 2;
	}
}

void CPU::TYA(uint8_t opCode) {
	_regA = _regY;
	SetFlag(Flags::FLAG_Z, _regA == 0);
	SetFlag(Flags::FLAG_N, (_regA & 0x80) != 0);
	_pc += 1;
}

void CPU::TXS(uint8_t opCode) {
	_sp = _regX;
	_pc += 1;
}

void CPU::LDY(uint8_t opCode) {
	uint16_t addr = 0;

	if (opCode == 0xA0) {
		addr = ReadAddrMode_IMM();
		_pc += 2;
	} else if (opCode == 0xA4) {
		addr = ReadAddrMode_ZPG();
		_pc += 2;
	} else if (opCode == 0xAC) {
		addr = ReadAddrMode_ABS();
		_pc += 3;
	} else if (opCode == 0xB4) {
		addr = ReadAddrMode_ZPG_X();
		_pc += 2;
	} else if (opCode == 0xBC) {
		addr = ReadAddrMode_ABS_X(true);
		_pc += 3;
	}

	_regY = _bus->Read(addr);
	SetFlag(Flags::FLAG_Z, _regY == 0);
	SetFlag(Flags::FLAG_N, (_regY & 0x80) != 0);
}

void CPU::LDA(uint8_t opCode) {
	uint16_t addr = 0;

	if (opCode == 0xA1) {
		addr = ReadAddrMode_X_IND();
		_pc += 2;
	} else if (opCode == 0xA5) {
		addr = ReadAddrMode_ZPG();
		_pc += 2;
	} else if (opCode == 0xA9) {
		addr = ReadAddrMode_IMM();
		_pc += 2;
	} else if (opCode == 0xAD) {
		addr = ReadAddrMode_ABS();
		_pc += 3;
	} else if (opCode == 0xB1) {
		addr = ReadAddrMode_IND_Y(true);
		_pc += 2;
	} else if (opCode == 0xB5) {
		addr = ReadAddrMode_ZPG_X();
		_pc += 2;
	} else if (opCode == 0xB9) {
		addr = ReadAddrMode_ABS_Y(true);
		_pc += 3;
	} else if (opCode == 0xBD) {
		addr = ReadAddrMode_ABS_X(true);
		_pc += 3;
	}

	_regA = _bus->Read(addr);
	SetFlag(Flags::FLAG_Z, _regA == 0);
	SetFlag(Flags::FLAG_N, (_regA & 0x80) != 0);
}

void CPU::LDX(uint8_t opCode) {
	uint16_t addr = 0;

	if (opCode == 0xA2) {
		addr = ReadAddrMode_IMM();
		_pc += 2;
	} else if (opCode == 0xA6) {
		addr = ReadAddrMode_ZPG();
		_pc += 2;
	} else if (opCode == 0xAE) {
		addr = ReadAddrMode_ABS();
		_pc += 3;
	} else if (opCode == 0xB6) {
		addr = ReadAddrMode_ZPG_Y();
		_pc += 2;
	} else if (opCode == 0xBE) {
		addr = ReadAddrMode_ABS_Y(true);
		_pc += 3;
	}

	_regX = _bus->Read(addr);
	SetFlag(Flags::FLAG_Z, _regX == 0);
	SetFlag(Flags::FLAG_N, (_regX & 0x80) != 0);
}

void CPU::TAY(uint8_t opCode) {
	_regY = _regA;
	SetFlag(Flags::FLAG_Z, _regY == 0);
	SetFlag(Flags::FLAG_N, (_regY & 0x80) != 0);
	_pc += 1;
}

void CPU::TAX(uint8_t opCode) {
	_regX = _regA;
	SetFlag(Flags::FLAG_Z, _regX == 0);
	SetFlag(Flags::FLAG_N, (_regX & 0x80) != 0);
	_pc += 1;
}

void CPU::BCS(uint8_t opCode) {
	uint16_t offset = ReadAddrMode_REL(true);

	if (GetFlag(Flags::FLAG_C) == FLAG_SET)
		_pc += offset;
	else
		_pc += 2;
}

void CPU::CLV(uint8_t opCode) {
	SetFlag(Flags::FLAG_V, FLAG_CLEAR);
	_pc += 1;
}

void CPU::TSX(uint8_t opCode) {
	_regX = _sp;
	SetFlag(Flags::FLAG_Z, _regX == 0);
	SetFlag(Flags::FLAG_N, (_regX & 0x80) != 0);

	_pc += 1;
}

void CPU::CPY(uint8_t opCode) {
	uint16_t addr = 0;

	if (opCode == 0xC0) {
		addr = ReadAddrMode_IMM();
		_pc += 2;
	} else if (opCode == 0xC4) {
		addr = ReadAddrMode_ZPG();
		_pc += 2;
	} else if (opCode == 0xCC) {
		addr = ReadAddrMode_ABS();
		_pc += 3;
	}

	uint8_t memVal = _bus->Read(addr);
	SetFlag(Flags::FLAG_C, _regY >= memVal);
	SetFlag(Flags::FLAG_Z, _regY == memVal);

	// THIS MIGHT BE WRONG!!!!!!!!!!!!!!!
	SetFlag(Flags::FLAG_N, (_regY & 0x80) != 0);
}

void CPU::CMP(uint8_t opCode) {
	uint16_t addr = 0;

	if (opCode == 0xC1) {
		addr = ReadAddrMode_X_IND();
		_pc += 2;
	} else if (opCode == 0xC5) {
		addr = ReadAddrMode_ZPG();
		_pc += 2;
	} else if (opCode == 0xC9) {
		addr = ReadAddrMode_IMM();
		_pc += 2;
	} else if (opCode == 0xCD) {
		addr = ReadAddrMode_ABS();
		_pc += 3;
	} else if (opCode == 0xD1) {
		addr = ReadAddrMode_IND_Y(true);
		_pc += 2;
	} else if (opCode == 0xD5) {
		addr = ReadAddrMode_ZPG_X();
		_pc += 2;
	} else if (opCode == 0xD9) {
		addr = ReadAddrMode_ABS_Y(true);
		_pc += 3;
	} else if (opCode == 0xDD) {
		addr = ReadAddrMode_ABS_X(true);
		_pc += 3;
	}

	uint8_t memVal = _bus->Read(addr);
	SetFlag(Flags::FLAG_C, _regA >= memVal);
	SetFlag(Flags::FLAG_Z, _regA == memVal);

	// MIGHT BE WRONG!
	SetFlag(Flags::FLAG_N, (_regA & 0x80) != 0);
}

void CPU::DEC(uint8_t opCode) {
	uint16_t addr = 0;

	if (opCode == 0xC6) {
		addr = ReadAddrMode_ZPG();
		_pc += 2;
	} else if (opCode == 0xCE) {
		addr = ReadAddrMode_ABS();
		_pc += 3;
	} else if (opCode == 0xD6) {
		addr = ReadAddrMode_ZPG_X();
		_pc += 2;
	} else if (opCode == 0xDE) {
		addr = ReadAddrMode_ABS_X();
		_pc += 3;
	}

	uint8_t result = _bus->Read(addr) - 1;
	_bus->Write(addr, result);
	SetFlag(Flags::FLAG_Z, result == 0);
	SetFlag(Flags::FLAG_N, (result & 0x80) != 0);
}

void CPU::INY(uint8_t opCode) {
	_regY += 1;
	SetFlag(Flags::FLAG_Z, _regY == 0);
	SetFlag(Flags::FLAG_N, (_regY & 0x80) != 0);
	_pc += 1;
}

void CPU::DEX(uint8_t opCode) {
	_regX -= 1;
	SetFlag(Flags::FLAG_Z, _regX == 0);
	SetFlag(Flags::FLAG_N, (_regX & 0x80) != 0);
	_pc += 1;
}

void CPU::CPX(uint8_t opCode) {
	uint16_t addr = 0;

	if (opCode == 0xE0) {
		addr = ReadAddrMode_IMM();
		_pc += 2;
	} else if (opCode == 0xE4) {
		addr = ReadAddrMode_ZPG();
		_pc += 2;
	} else if (opCode == 0xEC) {
		addr = ReadAddrMode_ABS();
		_pc += 3;
	}

	uint8_t memVal = _bus->Read(addr);
	SetFlag(Flags::FLAG_C, _regX >= memVal);
	SetFlag(Flags::FLAG_Z, _regX == memVal);
	SetFlag(Flags::FLAG_N, (_regX & 0x80) != 0);
}

void CPU::SBC(uint8_t opCode) {
	uint16_t addr = 0;
	uint8_t val = 0;

	if (opCode == 0xE1) {
		addr = ReadAddrMode_X_IND();
		_pc += 2;
	} else if (opCode == 0xE5) {
		addr = ReadAddrMode_ZPG();
		_pc += 2;
	} else if (opCode == 0xE9) {
		addr = ReadAddrMode_IMM();
		_pc += 2;
	} else if (opCode == 0xED) {
		addr = ReadAddrMode_ABS();
		_pc += 3;
	} else if (opCode == 0xF1) {
		addr = ReadAddrMode_IND_Y(true);
		_pc += 2;
	} else if (opCode == 0xF5) {
		addr = ReadAddrMode_ZPG_X();
		_pc += 2;
	} else if (opCode == 0xF9) {
		addr = ReadAddrMode_ABS_Y(true);
		_pc += 3;
	} else if (opCode == 0xFD) {
		addr = ReadAddrMode_ABS_X(true);
		_pc += 3;
	}

	val = _bus->Read(addr);

	uint16_t result = static_cast<uint16_t>(_regA)
		- static_cast<uint16_t>(val)
		- static_cast<uint16_t>(GetFlag(Flags::FLAG_C) == FLAG_SET ? 0 : 1);

	_regA = static_cast<uint8_t>(result);

	// PROBABLY WRONG
	SetFlag(Flags::FLAG_C, result > 0x00FF);
	SetFlag(Flags::FLAG_Z, _regA == 0);
	SetFlag(Flags::FLAG_V,
		(~(static_cast<uint16_t>(_regA) ^ static_cast<uint16_t>(val))
			& (static_cast<uint16_t>(_regA) ^ static_cast<uint16_t>(result))
			& 0x0080)
		== 0);
	SetFlag(Flags::FLAG_N, (_regA & 0x80) != 0);
}

void CPU::INC(uint8_t opCode) {
	uint16_t addr = 0;

	if (opCode == 0xE6) {
		addr = ReadAddrMode_ZPG();
		_pc += 2;
	} else if (opCode == 0xEE) {
		addr = ReadAddrMode_ABS();
		_pc += 3;
	} else if (opCode == 0xF6) {
		addr = ReadAddrMode_ZPG_X();
		_pc += 2;
	} else if (opCode == 0xFE) {
		addr = ReadAddrMode_ABS_X();
		_pc += 3;
	}

	uint8_t result = _bus->Read(addr) + 1;
	_bus->Write(addr, result);
	SetFlag(Flags::FLAG_Z, result == 0);
	SetFlag(Flags::FLAG_N, (result & 0x80) != 0);

}

void CPU::INX(uint8_t opCode) {
	_regX += 1;
	SetFlag(Flags::FLAG_Z, _regX == 0);
	SetFlag(Flags::FLAG_N, (_regX & 0x80) != 0);

	_pc += 1;
}

void CPU::NOP(uint8_t opCode) {
	_pc += 1;
}

void CPU::BNE(uint8_t opCode) {
	if (GetFlag(Flags::FLAG_Z) == FLAG_CLEAR)
		_pc += ReadAddrMode_REL();
	else
		_pc += 2;
}

void CPU::CLD(uint8_t opCode) {
	SetFlag(Flags::FLAG_D, FLAG_CLEAR);
	_pc += 1;
}

void CPU::BEQ(uint8_t opCode) {
	if (GetFlag(Flags::FLAG_Z) == FLAG_SET)
		_pc += ReadAddrMode_REL(true);
	else
		_pc += 2;
}

void CPU::SED(uint8_t opCode) {
	SetFlag(Flags::FLAG_D, FLAG_SET);
	_pc += 1;
}