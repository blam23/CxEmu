add_executable (CxEmu "main.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET CxEmu PROPERTY CXX_STANDARD 20)
endif()

include_directories("../")
target_link_libraries (CxEmu nes)

