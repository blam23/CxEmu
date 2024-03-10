#include "core/types.h"

namespace cx
{

class emulator_base
{
  public:
    emulator_base(u32 cycles_per_frame, f64 frames_per_second)
        : m_cycles_per_frame(cycles_per_frame), m_frames_per_second(frames_per_second)
    {
    }

    virtual ~emulator_base()
    {
    }

    virtual auto clock() -> u8 = 0;
    virtual void reset() = 0;
    virtual void shudown() = 0;

    auto constexpr cycles_per_frame() const
    {
        return m_cycles_per_frame;
    }
    auto constexpr frames_per_second() const
    {
        return m_frames_per_second;
    }

  private:
    u32 m_cycles_per_frame;
    f64 m_frames_per_second;
};

} // namespace cx
