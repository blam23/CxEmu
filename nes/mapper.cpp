#include "mapper.h"
#include "emulator.h"
#include "mappers/mmc1.h"
#include "mappers/nrom.h"

namespace cx::nes
{

auto mapper::create(u8 number, emulator* system) -> std::unique_ptr<mapper>
{
    if (!s_registered)
    {
        register_mappers();
        s_registered = true;
    }

    auto itr{ s_factories.find(number) };
    if (itr == s_factories.end())
        throw std::exception{ "Mapper not supported." };

    return itr->second(system);
}

void mapper::register_mappers()
{
    cx::nes::mapper::register_factory(0, [](emulator* system) { return std::make_unique<cx::nes::nrom>(system); });
    cx::nes::mapper::register_factory(1, [](emulator* system) { return std::make_unique<cx::nes::mmc1>(system); });
}

} // namespace cx::nes
