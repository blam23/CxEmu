#include "mapper.h"
#include "mappers/nrom.h"

namespace cx::nes
{

void mapper::register_mappers()
{
    cx::nes::mapper::register_factory(0, [](cart* cart) { return std::make_unique<cx::nes::nrom>(cart); });
}

} // namespace cx::nes
