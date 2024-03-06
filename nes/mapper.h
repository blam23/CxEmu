#pragma once

#include "cart.h"
#include "core/types.h"
#include <functional>
#include <map>

namespace cx::nes
{
class mapper
{
  public:
    virtual ~mapper()
    {
    }

    virtual u8 read(u16 addr) = 0;
    virtual void write(u16 addr, u8 value) = 0;

    virtual bool is_irq_set() = 0;
    virtual u8 read_chr_rom(u16 addr) = 0;
    virtual void write_chr_rom(u16 addr, u8 value) = 0;

    static void register_mappers();

    static std::unique_ptr<mapper> create(u8 number, cart* cart)
    {
        if (!s_registered)
        {
            register_mappers();
            s_registered = true;
        }

        auto itr{ s_factories.find(number) };
        if (itr == s_factories.end())
            throw std::exception{ "Mapper not supported." };

        return itr->second(cart);
    }

    using factory = std::function<std::unique_ptr<mapper>(cart* cart)>;
    static void register_factory(u8 number, factory factory)
    {
        s_factories[number] = factory;
    }

  private:
    static inline std::map<u8, factory> s_factories{};
    static inline bool s_registered{ false };
};
} // namespace cx::nes