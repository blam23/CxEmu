#pragma once

#include "core/types.h"
#include "forward.h"
#include <functional>
#include <map>
#include <memory>

namespace cx::gbc
{

// Memory bank controller (IMBC). Concrete mappers live in cx::gbc::mappers and are registered through
// their [[=mbc{ cart_type }]] annotations (see register_mappers).
class mapper
{
  public:
    virtual ~mapper()
    {
    }

    virtual auto read(u16 addr) -> u8 = 0;
    virtual auto write(u16 addr, u8 value) -> void = 0;
    virtual auto shutdown() -> void
    {
    }

    static auto register_mappers() -> void;

    static auto create(u8 cart_type, emulator* system) -> std::unique_ptr<mapper>;

    using factory = std::function<std::unique_ptr<mapper>(emulator* system)>;
    static auto register_factory(u8 cart_type, factory factory) -> void;

  private:
    static inline std::map<u8, factory> s_factories{};
    static inline bool s_registered{ false };
};

} // namespace cx::gbc
