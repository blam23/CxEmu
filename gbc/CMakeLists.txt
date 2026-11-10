add_library (
  gbc
  "emulator.cpp"
  "cart.cpp"
  "cpu.cpp"
  "op_codes.cpp"
  "ppu.cpp"
  "fifo.cpp"
  "palette.cpp"
  "memory_bus.cpp"
  "dma.cpp"
  "timer.cpp"
  "joypad.cpp"
  "mapper.cpp"
  "mappers/no_mbc.cpp"
  "mappers/mbc1.cpp"
  "mappers/mbc3.cpp"
)

# C++26 reflection (GCC 16). PUBLIC because the headers carry [[=annotations]].
target_compile_features(gbc PUBLIC cxx_std_26)
target_compile_options(gbc PUBLIC -freflection)

include_directories("../")

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(gbc PRIVATE spdlog::spdlog)
