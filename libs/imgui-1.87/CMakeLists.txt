add_library(imgui imgui.cpp imgui_draw.cpp imgui_tables.cpp imgui_widgets.cpp)

add_subdirectory(backends)

target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})