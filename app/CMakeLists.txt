add_executable (CxEmu "main.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET CxEmu PROPERTY CXX_STANDARD 23)
endif()

include_directories("../")

include("D:/vcpkg/scripts/buildsystems/vcpkg.cmake")

find_package(SDL2 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
target_link_libraries(CxEmu PRIVATE nes spdlog::spdlog $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main> $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>)