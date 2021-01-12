#pragma once

struct DebugCPUInfo{
    uint16_t pc;
    uint16_t sp;
    uint8_t regA;
    uint8_t regX;
    uint8_t regY;
    uint8_t flags;
};

class Debugger {
    private:
        static DebugCPUInfo _cpuInfo;
    public:
        Debugger(){}

        void Render();

        void DrawViewMemory();
        void DrawViewCPU();
        void DrawViewConsole();

        static void SetCPUInfo(uint16_t pc, uint16_t sp, uint8_t regA, uint8_t regX, uint8_t regY, uint8_t flags);

};