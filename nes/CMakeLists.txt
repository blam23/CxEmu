add_library (nes "emulator.cpp" "cart.cpp" "cpu.cpp" "op_codes.cpp" "apu.cpp" "ppu.cpp" "ppu_bus.cpp" "mapper.cpp" "cpu_bus.cpp" "mappers/nrom.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET nes PROPERTY CXX_STANDARD 20)
endif()

include_directories("../")

