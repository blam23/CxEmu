add_library (core "core.cpp" "types.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET core PROPERTY CXX_STANDARD 23)
endif()

include_directories("../")
