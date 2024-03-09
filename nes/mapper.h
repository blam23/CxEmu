#pragma once

#include "core/types.h"
#include "forward.h"
#include <functional>
#include <map>
#include <memory>

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

    virtual u8 read_chr_rom(u16 addr) = 0;
    virtual void write_chr_rom(u16 addr, u8 value) = 0;

    virtual bool is_irq_set() = 0;
    virtual void scan_line() = 0;

    static void register_mappers();

    static auto create(u8 number, emulator* system) -> std::unique_ptr<mapper>;

    using factory = std::function<std::unique_ptr<mapper>(emulator* system)>;
    static void register_factory(u8 number, factory factory)
    {
        s_factories[number] = factory;
    }

  private:
    static inline std::map<u8, factory> s_factories{};
    static inline bool s_registered{ false };
};
} // namespace cx::nes