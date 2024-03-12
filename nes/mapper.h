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

    virtual auto read(u16 addr) const -> u8 = 0;
    virtual auto write(u16 addr, u8 value) -> void = 0;

    virtual auto read_chr_rom(u16 addr) const -> u8 = 0;
    virtual auto write_chr_rom(u16 addr, u8 value) -> void = 0;

    virtual auto is_irq_set() -> bool = 0;
    virtual auto scan_line() -> void = 0;

    static auto register_mappers() -> void;

    static auto create(u8 number, emulator* system) -> std::unique_ptr<mapper>;

    using factory = std::function<std::unique_ptr<mapper>(emulator* system)>;
    static auto register_factory(u8 number, factory factory) -> void
    {
        s_factories[number] = factory;
    }

  private:
    static inline std::map<u8, factory> s_factories{};
    static inline bool s_registered{ false };
};
} // namespace cx::nes