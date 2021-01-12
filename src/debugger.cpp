#include "debugger.h"
#include "screen.h"

DebugCPUInfo Debugger::_cpuInfo;

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


    ImGui::End();
}

void Debugger::DrawViewConsole(){
    ImGui::SetNextWindowSize(ImVec2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2));
    ImGui::SetNextWindowPos(ImVec2(SCREEN_START_X + SCREEN_WIDTH / 2, SCREEN_START_Y + SCREEN_HEIGHT / 2));
    ImGui::Begin("Console View");
    ImGui::Text("Console info");
    ImGui::End();
}

void Debugger::SetCPUInfo(uint16_t pc, uint16_t sp, uint8_t regA, uint8_t regX, uint8_t regY, uint8_t flags){
    _cpuInfo.pc = pc;
    _cpuInfo.sp = sp;
    _cpuInfo.regA = regA;
    _cpuInfo.regX = regX;
    _cpuInfo.regY = regY;
    _cpuInfo.flags = flags;
}