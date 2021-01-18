#include "emulator.h"

int main(int argc, char *argv[]) {
    Emulator emulator;

    // Configure settings
    // Can do command line options here later
    emulator.settings.SetShowDebugView(true);

    emulator.Start();
}