#pragma once

struct DebugCPUInfo {
    const char* mnemonic;
    uint8_t opCode;
    uint16_t pc;
    uint16_t sp;
    uint8_t regA;
    uint8_t regX;
    uint8_t regY;
    uint8_t flags;
};

enum class ConsoleEntryType{
    CET_STANDARD,
    CET_WARNING,
    CET_ERROR
};

struct ConsoleEntry {
    tm time;
    std::string message;
    ConsoleEntryType messageType;
};

class Debugger {
    private:
        static DebugCPUInfo _cpuInfo;
        static std::vector<ConsoleEntry> _messages;
    public:
        Debugger(){
            _cpuInfo.mnemonic = "";
            _cpuInfo.opCode = 0;
            _cpuInfo.pc = 0;
            _cpuInfo.sp = 0;
            _cpuInfo.regA = 0;
            _cpuInfo.regX = 0;
            _cpuInfo.regY = 0;
            _cpuInfo.flags = 0;
        }

        void Render();

        void DrawViewMemory();
        void DrawViewCPU();
        void DrawViewConsole();

        static void SetCPUInfo(uint8_t opCode, const char* mnemonic, uint16_t pc, uint16_t sp, uint8_t regA, uint8_t regX, uint8_t regY, uint8_t flags);
        static void LogMessage(std::string message);
        static void LogWarning(std::string warning);
        static void LogError(std::string error);
    private:
        static void PushEntry(std::string message, ConsoleEntryType msgType);
};