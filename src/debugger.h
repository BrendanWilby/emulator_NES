#pragma once

class Bus;
class CPU;

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
        static std::vector<ConsoleEntry> _messages;
        CPU* _cpu;
        Bus* _bus;
    public:
        Debugger(CPU& cpu, Bus& bus) :
            _cpu(&cpu),
            _bus(&bus)
        {}

        void Render();

        void DrawViewMemory();
        void DrawViewCPU();
        void DrawViewConsole();

        static void LogMessage(std::string message);
        static void LogWarning(std::string warning);
        static void LogError(std::string error);
    private:
        static void PushEntry(std::string message, ConsoleEntryType msgType);
};