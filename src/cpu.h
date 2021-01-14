#pragma once

constexpr auto NUM_INSTRUCTIONS = 256;
constexpr auto FLAG_CLEAR = false;
constexpr auto FLAG_SET = true;
using FlagStatus = bool;

class Bus;

class CPU {
    private:
		uint8_t _currentOpCode;
		const char* _currentOpMnemonic;

        uint16_t _pc;
        uint8_t _sp;		// An offset from 0x0100, in the range 0x00 -> 0xFF
        uint8_t _regA;
        uint8_t _regX;
        uint8_t _regY;
        uint8_t _flags;

		uint16_t _lastPC;

        Bus* _bus;
        uint8_t _addCycles;	// Additional cycles to add

        struct CPUInstructions {
		const char* mnemonic;
		uint8_t cycles;
		void(CPU::* execute)(uint8_t);
	};

	const CPUInstructions _instructions[NUM_INSTRUCTIONS] = {
		// 0x00
		{ "BRK",		7, &CPU::BRK },
		{ "ORA X, ind", 6, &CPU::ORA },
		{ "NOOP",		2, nullptr },
		{ "NOOP",		2, &CPU::NOP },
		{ "NOOP",		2, &CPU::NOP },
		{ "ORA zpg",	3, &CPU::ORA },
		{ "ASL zpg",	5, &CPU::ASL },
		{ "NOOP",		2, &CPU::NOP },
		{ "PHP imp",	3, &CPU::PHP },
		{ "ORA imm",	2, &CPU::ORA },
		{ "ASL A",		2, &CPU::ASL },
		{ "NOOP",		2, &CPU::NOP },
		{ "NOOP",		2, &CPU::NOP },
		{ "ORA abs",	4, &CPU::ORA },
		{ "ASL abs",	6, &CPU::ASL },
		{ "NOOP",		2, &CPU::NOP },

		// 0x01
		{ "BPL rel",	2, &CPU::BPL },
		{ "ORA ind, Y", 5, &CPU::ORA },
		{ "NOOP",		2, &CPU::NOP },
		{ "NOOP",		2, &CPU::NOP },
		{ "NOOP",		2, &CPU::NOP },
		{ "ORA zpg, X", 4, &CPU::ORA },
		{ "ASL zpg, X", 6, &CPU::ASL },
		{ "NOOP",		2, &CPU::NOP },
		{ "CLC impl",	2, &CPU::CLC },
		{ "ORA abs, Y",	4, &CPU::ORA },
		{ "NOOP",		2, &CPU::NOP },
		{ "NOOP",		2, &CPU::NOP },
		{ "NOOP",		2, &CPU::NOP },
		{ "ORA abs, X",	4, &CPU::ORA },
		{ "ASL abs, X",	7, &CPU::ASL },
		{ "NOOP",		2, &CPU::NOP },

		// 0x02
		{ "JSR abs",	6, &CPU::JSR },
		{ "AND X, ind", 6, &CPU::AND },
		{ "NOOP",		2, &CPU::NOP },
		{ "NOOP",		2, &CPU::NOP },
		{ "BIT zpg",	3, &CPU::BIT },
		{ "AND zpg",    3, &CPU::AND },
		{ "ROL zpg",    5, &CPU::ROL },
		{ "NOOP",		2, &CPU::NOP },
		{ "PLP impl",	4, &CPU::PLP },
		{ "AND #",		2, &CPU::AND },
		{ "ROL A",		2, &CPU::ROL },
		{ "NOOP",		2, &CPU::NOP },
		{ "BIT abs",	4, &CPU::BIT },
		{ "AND abs",	4, &CPU::AND },
		{ "ROL abs",	6, &CPU::ROL },
		{ "NOOP",		2, &CPU::NOP },

		// 0x03
		{ "BMI rel",	2, &CPU::BMI },
		{ "AND ind, Y",	5, &CPU::AND },
		{ "NOOP",		2, &CPU::NOP },
		{ "NOOP",		2, &CPU::NOP },
		{ "NOOP",		2, &CPU::NOP },
		{ "AND zpg, X",	4, &CPU::AND },
		{ "ROL zpg, X",	6, &CPU::ROL },
		{ "NOOP",		2, &CPU::NOP },
		{ "SEC impl",	2, &CPU::SEC },
		{ "AND abs, Y",	4, &CPU::AND },
		{ "NOOP",		2, &CPU::NOP },
		{ "NOOP",		2, &CPU::NOP },
		{ "NOOP",		2, &CPU::NOP },
		{ "AND abs, X",	4, &CPU::AND },
		{ "ROL abs, X",	7, &CPU::ROL },
		{ "NOOP",		2, &CPU::NOP },

		// 0x04
		{ "RTI impl",	6, &CPU::RTI },
		{ "EOR X, ind", 6, &CPU::EOR },
		{ "NOOP",		2, &CPU::NOP },
		{ "NOOP",		2, &CPU::NOP },
		{ "NOOP",		2, &CPU::NOP },
		{ "EOR zpg",	3, &CPU::EOR },
		{ "LSR zpg",	5, &CPU::LSR },
		{ "NOOP",		2, &CPU::NOP },
		{ "PHA impl",   3, &CPU::PHA },
		{ "EOR #",		2, &CPU::EOR },
		{ "LSR A",		2, &CPU::LSR },
		{ "NOOP",		2, &CPU::NOP },
		{ "JMP abs",    3, &CPU::JMP },
		{ "EOR abs",    4, &CPU::EOR },
		{ "LSR abs",    6, &CPU::LSR },
		{ "NOOP",		2, &CPU::NOP },

		// 0x05
		{ "BVC rel",	2, &CPU::BVC },
		{ "EOR ind, Y", 5, &CPU::EOR },
		{ "NOOP",		2, &CPU::NOP },
		{ "NOOP",		2, &CPU::NOP },
		{ "NOOP",		2, &CPU::NOP },
		{ "EOR zpg, X", 4, &CPU::EOR },
		{ "LSR zpg, X", 6, &CPU::LSR },
		{ "NOOP",		2, &CPU::NOP },
		{ "CLI impl",	2, &CPU::CLI },
		{ "EOR abs, Y", 4, &CPU::EOR },
		{ "NOOP",		2, &CPU::NOP },
		{ "NOOP",		2, &CPU::NOP },
		{ "NOOP",		2, &CPU::NOP },
		{ "EOR abs, X", 4, &CPU::EOR },
		{ "LSR abs, X", 4, &CPU::LSR },
		{ "NOOP",		2, &CPU::NOP },

		// 0x06
		{ "RTS impl",	6, &CPU::RTS },
		{ "ADC X, ind", 6, &CPU::ADC },
		{ "NOOP",		2, &CPU::NOP },
		{ "NOOP",		2, &CPU::NOP },
		{ "NOOP",		2, &CPU::NOP },
		{ "ADC zpg",	3, &CPU::ADC },
		{ "ROR zpg",    5, &CPU::ROR },
		{ "NOOP",		2, &CPU::NOP },
		{ "PLA impl",	4, &CPU::PLA },
		{ "ADC #",      2, &CPU::ADC },
		{ "ROR A",      2, &CPU::ROR },
		{ "NOOP",		2, &CPU::NOP },
		{ "JMP ind",	5, &CPU::JMP },
		{ "ADC abs",	4, &CPU::ADC },
		{ "ROR abs",	6, &CPU::ROR },
		{ "NOOP",		2, &CPU::NOP },

		// 0x07
		{ "BVS rel",	2, &CPU::BVS },
		{ "ADC ind, Y",	5, &CPU::ADC },
		{ "NOOP",		2, &CPU::NOP },
		{ "NOOP",		2, &CPU::NOP },
		{ "NOOP",		2, &CPU::NOP },
		{ "ADC zpg, X",	4, &CPU::ADC },
		{ "ROR zpg, X",	4, &CPU::ROR },
		{ "NOOP",		2, &CPU::NOP },
		{ "SEI impl",	2, &CPU::SEI },
		{ "ADC abs, Y",	4, &CPU::ADC },
		{ "NOOP",		2, &CPU::NOP },
		{ "NOOP",		2, &CPU::NOP },
		{ "NOOP",		2, &CPU::NOP },
		{ "ADC abs, X",	4, &CPU::ADC },
		{ "ROR abs, X",	7, &CPU::ROR },
		{ "NOOP",		2, &CPU::NOP },

		// 0x08
		{ "NOOP",		2, &CPU::NOP },
		{ "STA X, ind",	6, &CPU::STA },
		{ "NOOP",		2, &CPU::NOP },
		{ "NOOP",		2, &CPU::NOP },
		{ "STY zpg",	3, &CPU::STY },
		{ "STA zpg",	3, &CPU::STA },
		{ "STX zpg",	3, &CPU::STX },
		{ "NOOP",		2, &CPU::NOP },
		{ "DEY impl",	2, &CPU::DEY },
		{ "NOOP",		2, &CPU::NOP },
		{ "TXA impl",	2, &CPU::TXA },
		{ "NOOP",		2, &CPU::NOP },
		{ "STY abs",	3, &CPU::STY },
		{ "STA abs",	4, &CPU::STA },
		{ "STX abs",	4, &CPU::STX },
		{ "NOOP",		2, &CPU::NOP },

		// 0x09
		{ "BCC rel",	2, &CPU::BCC },
		{ "STA ind, Y",	6, &CPU::STA },
		{ "NOOP",		2, &CPU::NOP },
		{ "NOOP",		2, &CPU::NOP },
		{ "STY zpg, X",	4, &CPU::STY },
		{ "STA zpg, X",	4, &CPU::STA },
		{ "STX zpg, Y",	4, &CPU::STX },
		{ "NOOP",		2, &CPU::NOP },
		{ "TYA impl",	2, &CPU::TYA },
		{ "STA abs, Y",	5, &CPU::STA },
		{ "TXS impl",	2, &CPU::TXS },
		{ "NOOP",		2, &CPU::NOP },
		{ "NOOP",		2, &CPU::NOP },
		{ "STA abs, X",	5, &CPU::STA },
		{ "NOOP",		2, &CPU::NOP },
		{ "NOOP",		2, &CPU::NOP },

		// 0xA0
		{ "LDY #",		2, &CPU::LDY},
		{ "LDA X, ind",	6, &CPU::LDA },
		{ "LDX #",		2, &CPU::LDX },
		{ "NOOP",		2, &CPU::NOP },
		{ "LDY zpg",	3, &CPU::LDY },
		{ "LDA zpg",	3, &CPU::LDA },
		{ "LDX zpg",	3, &CPU::LDX },
		{ "NOOP",		2, &CPU::NOP },
		{ "TAY impl",	2, &CPU::TAY },
		{ "LDA #",		2, &CPU::LDA },
		{ "TAX impl",	2, &CPU::TAX },
		{ "NOOP",		2, &CPU::NOP },
		{ "LDY abs",	4, &CPU::LDY },
		{ "LDA abs",	4, &CPU::LDA },
		{ "LDX abs",	4, &CPU::LDX },
		{ "NOOP",		2, &CPU::NOP },

		// 0xB0
		{ "BCS rel",	2, &CPU::BCS },
		{ "LDA ind, Y",	5, &CPU::LDA },
		{ "NOOP",		2, &CPU::NOP },
		{ "NOOP",		2, &CPU::NOP },
		{ "LDY zpg, X",	4, &CPU::LDY },
		{ "LDA zpg, X",	4, &CPU::LDA },
		{ "LDX zpy, Y",	4, &CPU::LDX },
		{ "NOOP",		2, &CPU::NOP },
		{ "CLV impl",	2, &CPU::CLV },
		{ "LDA abs Y",	4, &CPU::LDA },
		{ "TSX impl",	2, &CPU::TSX },
		{ "NOOP",		2, &CPU::NOP },
		{ "LDY abs, X",	4, &CPU::LDY },
		{ "LDA abs, X",	4, &CPU::LDA },
		{ "LDX abs, Y",	4, &CPU::LDX },
		{ "NOOP",		2, &CPU::NOP },

		// 0xC0
		{ "CPY #",		2, &CPU::CPY },
		{ "CMP X, ind",	6, &CPU::CMP },
		{ "NOOP",		2, &CPU::NOP },
		{ "NOOP",		2, &CPU::NOP },
		{ "CPY zpg",	3, &CPU::CPY },
		{ "CMP zpg",	3, &CPU::CMP },
		{ "DEC zpg",	5, &CPU::DEC },
		{ "NOOP",		2, &CPU::NOP },
		{ "INY impl",	2, &CPU::INY },
		{ "CMP #",		2, &CPU::CMP },
		{ "DEX impl",	2, &CPU::DEX },
		{ "NOOP",		2, &CPU::NOP },
		{ "CPY abs",	4, &CPU::CPY },
		{ "CMP abs",	4, &CPU::CMP },
		{ "DEC abs",	6, &CPU::DEC },
		{ "NOOP",		2, &CPU::NOP },

		// 0xD0
		{ "BNE rel",	2, &CPU::BNE },
		{ "CMP ind, Y",	5, &CPU::CMP },
		{ "NOOP",		2, &CPU::NOP },
		{ "NOOP",		2, &CPU::NOP },
		{ "NOOP",		2, &CPU::NOP },
		{ "CMP zpg, X",	4, &CPU::CMP },
		{ "DEC zpg, X",	6, &CPU::DEC },
		{ "NOOP",		2, &CPU::NOP },
		{ "CLD impl",	2, &CPU::CLD },
		{ "CMP abs, Y",	4, &CPU::CMP },
		{ "NOOP",		2, &CPU::NOP },
		{ "NOOP",		2, &CPU::NOP },
		{ "NOOP",		2, &CPU::NOP },
		{ "CMP abs, X",	4, &CPU::CMP },
		{ "DEC abs, X",	7, &CPU::DEC },
		{ "NOOP",		2, &CPU::NOP },

		// 0xE0
		{ "CPX #",		2, &CPU::CPX },
		{ "SBC X, ind",	6, &CPU::SBC },
		{ "NOOP",		2, &CPU::NOP },
		{ "NOOP",		2, &CPU::NOP },
		{ "CPX zpg",	3, &CPU::CPX },
		{ "SBC zpg",	3, &CPU::SBC },
		{ "INC zpg",	5, &CPU::INC },
		{ "NOOP",		2, &CPU::NOP },
		{ "INX impl",	2, &CPU::INX },
		{ "SBC #",		2, &CPU::SBC },
		{ "NOP impl",	2, &CPU::NOP },
		{ "NOOP",		2, &CPU::NOP },
		{ "CPX abs",	4, &CPU::CPX },
		{ "SBC abs",	4, &CPU::SBC },
		{ "INC abs",	6, &CPU::INC },
		{ "NOOP",		2, &CPU::NOP },

		// 0xF0
		{ "BEQ rel",    2, &CPU::BEQ },
		{ "SBC ind, Y", 5, &CPU::SBC },
		{ "NOOP",		2, &CPU::NOP },
		{ "NOOP",		2, &CPU::NOP },
		{ "NOOP",		2, &CPU::NOP },
		{ "SBC zpg, X", 4, &CPU::SBC },
		{ "INC zpg, X", 6, &CPU::INC },
		{ "NOOP",		2, &CPU::NOP },
		{ "SED impl",	2, &CPU::SED },
		{ "SBC abs, Y",	4, &CPU::SBC },
		{ "NOOP",		2, &CPU::NOP },
		{ "NOOP",		2, &CPU::NOP },
		{ "NOOP",		2, &CPU::NOP },
		{ "SBC abs, X",	4, &CPU::SBC },
		{ "INC abs, X",	7, &CPU::INC },
		{ "NOOP",		2, &CPU::NOP },
	};
    
    public:
        CPU() : _pc(0),
                _sp(0),
                _regA(0),
                _regX(0),
                _regY(0),
                _flags(0),
                _addCycles(0)
        {
        }

		enum class Flags {
			FLAG_C = (1 << 0),
			FLAG_Z = (1 << 1),
			FLAG_I = (1 << 2),
			FLAG_D = (1 << 3),
			FLAG_B = (1 << 4),
			FLAG_V = (1 << 6),
			FLAG_N = (1 << 7)
		};

        uint8_t Execute();
        void Reset();
        void ConnectToBus(Bus &bus);

		uint8_t GetOpCode(){ return _currentOpCode; }
		const char* GetOpMnemonic(){ return _currentOpMnemonic; }
		uint16_t GetPC(){ return _pc; }
		uint16_t GetLastPC(){ return _lastPC; }
		uint8_t GetSP(){ return _sp; }
		uint8_t GetRegA(){ return _regA; }
		uint8_t GetRegX(){ return _regX; }
		uint8_t GetRegY(){ return _regY; }
        FlagStatus GetFlag(Flags flag);
		uint8_t GetFlags() { return _flags; }

    private:
        void SetFlag(Flags flag, FlagStatus status);

        void PushStack8(uint8_t value);
        void PushStack16(uint16_t value);
        uint8_t PopStack8();
        uint16_t PopStack16();

        bool CheckPageChange(uint16_t addrOld, uint16_t addrNew);

        /*
            Addressing Modes
        */

        // Reads the immediate 16-bit value as an address
        uint16_t ReadAddrMode_ABS();
        uint16_t ReadAddrMode_ABS_X(bool checkPage=false);
        uint16_t ReadAddrMode_ABS_Y(bool checkPage=false);
        uint16_t ReadAddrMode_IMM();
        uint16_t ReadAddrMode_IND();
        // Address is immediate 8-bit value added to x register
        uint16_t ReadAddrMode_X_IND();
        uint16_t ReadAddrMode_IND_Y(bool checkPage=false);
        uint16_t ReadAddrMode_REL(bool checkPage=false);
        uint16_t ReadAddrMode_ZPG();
        uint16_t ReadAddrMode_ZPG_X();
        uint16_t ReadAddrMode_ZPG_Y();

        /*
            Instructions
        */

        void BRK(uint8_t opCode);
        void ORA(uint8_t opCode);
        void ASL(uint8_t opCode);
        void PHP(uint8_t opCode);
        void BPL(uint8_t opCode);
        void CLC(uint8_t opCode);
        void JSR(uint8_t opCode);
        void AND(uint8_t opCode);
        void BIT(uint8_t opCode);
        void ROL(uint8_t opCode);
        void PLP(uint8_t opCode);
        void BMI(uint8_t opCode);
        void SEC(uint8_t opCode);
        void RTI(uint8_t opCode);
        void EOR(uint8_t opCode);
        void LSR(uint8_t opCode);
        void PHA(uint8_t opCode);
        void JMP(uint8_t opCode);
        void BVC(uint8_t opCode);
        void CLI(uint8_t opCode);
        void RTS(uint8_t opCode);
        void ADC(uint8_t opCode);
        void ROR(uint8_t opCode);
        void PLA(uint8_t opCode);
        void BVS(uint8_t opCode);
        void SEI(uint8_t opCode);
        void STA(uint8_t opCode);
        void STY(uint8_t opCode);
        void STX(uint8_t opCode);
        void DEY(uint8_t opCode);
        void TXA(uint8_t opCode);
        void BCC(uint8_t opCode);
        void TYA(uint8_t opCode);
        void TXS(uint8_t opCode);
        void LDY(uint8_t opCode);
        void LDA(uint8_t opCode);
        void LDX(uint8_t opCode);
        void TAY(uint8_t opCode);
        void TAX(uint8_t opCode);
        void BCS(uint8_t opCode);
        void CLV(uint8_t opCode);
        void TSX(uint8_t opCode);
        void CPY(uint8_t opCode);
        void CMP(uint8_t opCode);
        void DEC(uint8_t opCode);
        void INY(uint8_t opCode);
        void DEX(uint8_t opCode);
        void CPX(uint8_t opCode);
        void SBC(uint8_t opCode);
        void INC(uint8_t opCode);
        void INX(uint8_t opCode);
        void NOP(uint8_t opCode);
        void BNE(uint8_t opCode);
        void CLD(uint8_t opCode);
        void BEQ(uint8_t opCode);
        void SED(uint8_t opCode);
};