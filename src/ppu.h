#pragma once

class Bus;

class PPU {
    private:
        Bus* _bus;
    public:
        PPU(){}
        void Reset();
        void ConnectToBus(Bus &bus);

};