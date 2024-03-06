#include "nes/emulator.h"
#include <iostream>

int main()
{
    cx::nes::mapper::register_factory(255, nullptr);

    cx::nes::emulator emulator{ cx::nes::emulator::load_rom("D:/Test/NES/mario.nes") };

    u64 i{ 0 };
    while (true)
    {
        if (i % 3 == 0)
        {
            // TODO: prob should use ncurses or somethin
            auto key{ std::getchar() };

            if (key == 27)
                break;
        }

        emulator.clock();
        i++;
    }

    return 0;
}
