#include "mapper.h"
#include "emulator.h"
#include "reflection.h"
#include <format>
#include <meta>
#include <stdexcept>

// Reflection only sees declarations visible in this translation unit, so every mapper must be included here
#include "mappers/mbc1.h"
#include "mappers/mbc3.h"
#include "mappers/no_mbc.h"

namespace cx::gbc
{

auto mapper::register_factory(u8 cart_type, factory factory) -> void
{
    if (s_factories.contains(cart_type))
        throw std::runtime_error{ std::format("Duplicate MBC registered for number: 0x{:02X}", cart_type) };

    s_factories[cart_type] = std::move(factory);
}

auto mapper::create(u8 cart_type, emulator* system) -> std::unique_ptr<mapper>
{
    if (!s_registered)
    {
        register_mappers();
        s_registered = true;
    }

    const auto itr{ s_factories.find(cart_type) };
    if (itr == s_factories.end())
        throw std::runtime_error{ std::format("Cart type: '0x{:02X}' not loaded (likely not supported).", cart_type) };

    return itr->second(system);
}

auto mapper::register_mappers() -> void
{
    template for (constexpr auto type : std::define_static_array(
                      std::meta::members_of(^^cx::gbc::mappers, std::meta::access_context::unchecked())))
    {
        template for (constexpr auto annotation :
                      std::define_static_array(std::meta::annotations_of_with_type(type, ^^mbc)))
        {
            register_factory(
                std::meta::extract<mbc>(annotation).cart_type,
                [](emulator* system) -> std::unique_ptr<mapper> { return std::make_unique<typename[:type:]>(system); });
        }
    }
}

} // namespace cx::gbc
