#pragma once

class CPU;
class Bus;

enum class NESState{
    NES_STATE_PAUSED,
    NES_STATE_STEPPING,
    NES_STATE_RUNNING,
    NES_STATE_STOPPED
};

class NES{
    private:
        std::unique_ptr<Bus> _bus;
        std::unique_ptr<CPU> _cpu;
        NESState _currentState;
    public:
        NES();

        void Start(bool autoStart = true);
        void Pause();
        void Update();

        void Step();

        CPU* GetCPU(){ return _cpu.get(); }
        Bus* GetBus(){ return _bus.get(); }
};