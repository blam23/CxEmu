add_library (
  nes
  "emulator.cpp"
  "cart.cpp"
  "cpu.cpp"
  "op_codes.cpp"
  "apu.cpp"
  "ppu.cpp"
  "ppu_bus.cpp"
  "palette.cpp"
  "joypad.cpp"
  "mapper.cpp"
  "cpu_bus.cpp"
  "mappers/nrom.cpp"
  "mappers/mmc1.cpp"
  "mappers/uxrom.cpp"
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET nes PROPERTY CXX_STANDARD 23)
endif()

include_directories("../")

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(nes PRIVATE spdlog::spdlog)

