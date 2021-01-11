#pragma once

class CPU;
class Bus;

class NES{
    private:
        std::unique_ptr<Bus> _bus;
        std::unique_ptr<CPU> _cpu;
    public:
        NES();

        void Start();
        void Update();
};