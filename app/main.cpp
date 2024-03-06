#include "nes/emulator.h"
#include <iostream>

int main()
{
    cx::nes::mapper::register_factory(255, nullptr);

    cx::nes::emulator emulator{ cx::nes::emulator::load_rom("D:/Test/NES/mario.nes") };

    emulator.clock();

    return 0;
}
