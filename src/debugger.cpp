#include "debugger.h"
#include "screen.h"

DebugCPUInfo Debugger::_cpuInfo;
std::vector<ConsoleEntry> Debugger::_messages;

void Debugger::Render(){
    DrawViewMemory();
    DrawViewCPU();
    DrawViewConsole();
}

void Debugger::DrawViewMemory(){
    ImGui::SetNextWindowSize(ImVec2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2));
    ImGui::SetNextWindowPos(ImVec2(SCREEN_START_X + SCREEN_WIDTH / 2, SCREEN_START_Y));
    ImGui::Begin("Memory View");
    ImGui::Text("Memory info");
    ImGui::End();
}

void Debugger::DrawViewCPU(){
    ImGui::SetNextWindowSize(ImVec2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2));
    ImGui::SetNextWindowPos(ImVec2(SCREEN_START_X, SCREEN_START_Y + SCREEN_HEIGHT / 2));
    ImGui::Begin("CPU View");

    ImGui::Text("OP: 0x%.2X (%s)", _cpuInfo.opCode, _cpuInfo.mnemonic);
    ImGui::Text("PC: 0x%.4X", _cpuInfo.pc);
    ImGui::Text("SP: 0x%.4X", _cpuInfo.sp);

    ImGui::Separator();
    ImGui::Text("Registers");
    ImGui::Separator();
    ImGui::Text("A: 0x%.2X", _cpuInfo.regA);
    ImGui::Text("X: 0x%.2X", _cpuInfo.regX);
    ImGui::Text("Y: 0x%.2X", _cpuInfo.regY);

    ImGui::Separator();
    ImGui::Text("Flags");
    ImGui::Separator();

    ImGui::Text("C Z I D B   V N");
    ImGui::Text("%d %d %d %d %d %d %d %d",
        (_cpuInfo.flags & (1 << 0)) != 0,
        (_cpuInfo.flags & (1 << 1)) != 0,
        (_cpuInfo.flags & (1 << 2)) != 0,
        (_cpuInfo.flags & (1 << 3)) != 0,
        (_cpuInfo.flags & (1 << 4)) != 0,
        0,
        (_cpuInfo.flags & (1 << 6)) != 0,
        (_cpuInfo.flags & (1 << 7)) != 0
    );

    ImGui::End();
}

void Debugger::DrawViewConsole(){
    ImGui::SetNextWindowSize(ImVec2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2));
    ImGui::SetNextWindowPos(ImVec2(SCREEN_START_X + SCREEN_WIDTH / 2, SCREEN_START_Y + SCREEN_HEIGHT / 2));
    ImGui::Begin("Console View");
    
    for(ConsoleEntry& msg : _messages){
        switch(msg.messageType){
            case ConsoleEntryType::CET_STANDARD:
                ImGui::Text(msg.message.c_str());
            break;

            case ConsoleEntryType::CET_WARNING:
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0, 1.0f), "[Warning]: %s", msg.message.c_str());
            break;

            case ConsoleEntryType::CET_ERROR:
                ImGui::TextColored(ImVec4(1.0f, 0, 0, 1.0f), "[Error]: %s", msg.message.c_str());
            break;
        }
    }

    ImGui::End();
}

void Debugger::SetCPUInfo(uint8_t opCode, const char* mnemonic, uint16_t pc, uint16_t sp, uint8_t regA, uint8_t regX, uint8_t regY, uint8_t flags){
    _cpuInfo.opCode = opCode;
    _cpuInfo.mnemonic = mnemonic;
    _cpuInfo.pc = pc;
    _cpuInfo.sp = sp;
    _cpuInfo.regA = regA;
    _cpuInfo.regX = regX;
    _cpuInfo.regY = regY;
    _cpuInfo.flags = flags;
}

void Debugger::LogMessage(std::string message){
    PushEntry(message, ConsoleEntryType::CET_STANDARD);
}

void Debugger::LogWarning(std::string warning){
    PushEntry(warning, ConsoleEntryType::CET_WARNING);
}

void Debugger::LogError(std::string error){
    PushEntry(error, ConsoleEntryType::CET_ERROR);
}

void Debugger::PushEntry(std::string message, ConsoleEntryType msgType){
    std::cout << message << std::endl;
    ConsoleEntry entry = { message, msgType };
    _messages.push_back(entry);
}