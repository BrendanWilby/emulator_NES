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

    uint16_t currentPC = _nes->GetCPU()->GetInitPC();

    ImGui::BeginTabBar("Memory Tab View");
    if(ImGui::BeginTabItem("RAM")){
        for(int i = 0; i < RAM_SIZE; i++){

            if(i == currentPC){
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "%.2X", _nes->GetBus()->GetRAM()[i]);
            }else{
                ImGui::Text("%.2X", _nes->GetBus()->GetRAM()[i]);
            }

            if(i % 16 < 15)
                ImGui::SameLine();
        }
        ImGui::EndTabItem();
    }

    if(ImGui::BeginTabItem("ROM")){
        for(int i = 0; i < PRG_ROM_SIZE; i++){
            if(i + PRG_ROM_BANK_0_START == currentPC){
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "%.2X", _nes->GetBus()->GetROM()[i]);
            }else{
                ImGui::Text("%.2X", _nes->GetBus()->GetROM()[i]);
            }

            if(i % 16 < 15)
                ImGui::SameLine();
        }
        ImGui::EndTabItem();
    }

    ImGui::EndTabBar();

    ImGui::End();
}

void Debugger::DrawViewCPU(){
    ImGui::SetNextWindowSize(ImVec2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2));
    ImGui::SetNextWindowPos(ImVec2(SCREEN_START_X, SCREEN_START_Y + SCREEN_HEIGHT / 2));
    ImGui::Begin("CPU View");

    ImGui::Text("OP: 0x%.2X (%s)", _nes->GetCPU()->GetOpCode(), _nes->GetCPU()->GetOpMnemonic());
    ImGui::Text("PC: 0x%.4X", _nes->GetCPU()->GetInitPC());
    ImGui::Text("SP: 0x%.4X", _nes->GetCPU()->GetInitSP());

    ImGui::Separator();
    ImGui::Text("Registers");
    ImGui::Separator();
    ImGui::Text("A: 0x%.2X", _nes->GetCPU()->GetInitRegA());
    ImGui::Text("X: 0x%.2X", _nes->GetCPU()->GetInitRegX());
    ImGui::Text("Y: 0x%.2X", _nes->GetCPU()->GetInitRegY());

    ImGui::Separator();
    ImGui::Text("Flags");
    ImGui::Separator();

    ImGui::Text("C Z I D B    V N");

    for(int i = 0; i < 8; i++){
        ImGui::Text("%d", (_nes->GetCPU()->GetInitFlags() & (1 << i)) == 0 ? 0 : 1);
        ImGui::SameLine();
    }
    ImGui::Text(" = 0x%.2X\n", _nes->GetCPU()->GetInitFlags());

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

        ImGui::SetScrollHere(1.0f);
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