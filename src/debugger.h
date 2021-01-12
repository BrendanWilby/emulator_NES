#pragma once

class NES;

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
        NES* _nes;

    public:
        Debugger(NES& nes) :
            _nes(&nes)
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