#include "debugger.h"
#include "screen.h"
#include "bus.h"
#include "cpu.h"
#include "nes.h"

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

    for(int i = 0; i < RAM_SIZE; i++){
        uint16_t currentPC = _nes->GetCPU()->GetPC();
        uint16_t lastPC = _nes->GetCPU()->GetLastPC();

        if(i == currentPC){
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "%.2X", _nes->GetBus()->GetRAM()[i]);
        }else if(i == lastPC){
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "%.2X", _nes->GetBus()->GetRAM()[i]);
        }else{
            ImGui::Text("%.2X", _nes->GetBus()->GetRAM()[i]);
        }

        if(i % 16 < 15)
            ImGui::SameLine();
    }

    ImGui::End();
}

void Debugger::DrawViewCPU(){
    ImGui::SetNextWindowSize(ImVec2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2));
    ImGui::SetNextWindowPos(ImVec2(SCREEN_START_X, SCREEN_START_Y + SCREEN_HEIGHT / 2));
    ImGui::Begin("CPU View");

    ImGui::Text("OP: 0x%.2X (%s)", _nes->GetCPU()->GetOpCode(), _nes->GetCPU()->GetOpMnemonic());
    ImGui::Text("PC: 0x%.4X", _nes->GetCPU()->GetPC());
    ImGui::Text("SP: 0x%.4X", _nes->GetCPU()->GetSP());

    ImGui::Separator();
    ImGui::Text("Registers");
    ImGui::Separator();
    ImGui::Text("A: 0x%.2X", _nes->GetCPU()->GetRegA());
    ImGui::Text("X: 0x%.2X", _nes->GetCPU()->GetRegX());
    ImGui::Text("Y: 0x%.2X", _nes->GetCPU()->GetRegY());

    ImGui::Separator();
    ImGui::Text("Flags");
    ImGui::Separator();

    ImGui::Text("C Z I D B   V N");
    ImGui::Text("%d %d %d %d %d %d %d %d",
        (_nes->GetCPU()->GetFlags() & (1 << 0)) != 0,
        (_nes->GetCPU()->GetFlags() & (1 << 1)) != 0,
        (_nes->GetCPU()->GetFlags() & (1 << 2)) != 0,
        (_nes->GetCPU()->GetFlags() & (1 << 3)) != 0,
        (_nes->GetCPU()->GetFlags() & (1 << 4)) != 0,
        0,
        (_nes->GetCPU()->GetFlags() & (1 << 6)) != 0,
        (_nes->GetCPU()->GetFlags() & (1 << 7)) != 0
    );

    ImGui::Separator();
    ImGui::Text("State");
    ImGui::Separator();
    ImGui::Text("%s", _nes->GetCurrentState());

    ImGui::End();
}

void Debugger::DrawViewConsole(){
    ImGui::SetNextWindowSize(ImVec2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2));
    ImGui::SetNextWindowPos(ImVec2(SCREEN_START_X + SCREEN_WIDTH / 2, SCREEN_START_Y + SCREEN_HEIGHT / 2));
    ImGui::Begin("Console View");
    
    for(ConsoleEntry& msg : _messages){
        switch(msg.messageType){
            case ConsoleEntryType::CET_STANDARD:
                ImGui::Text("[%.2d:%.2d:%.2d] %s", msg.time.tm_hour, msg.time.tm_min, msg.time.tm_sec, msg.message.c_str());
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
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    time_t time = std::chrono::system_clock::to_time_t(now);
    tm localTime = *localtime(&time);

    std::cout << message << std::endl;
    ConsoleEntry entry = { localTime, message, msgType };
    _messages.push_back(entry);
}