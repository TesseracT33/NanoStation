cmake_minimum_required(VERSION 3.13)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE .)

##### asmjit ####################
set(ASMJIT_BUILD_TEST OFF)
set(ASMJIT_STATIC ON)
set(ASMJIT_BUILD_X86 X64)
set(ASMJIT_BUILD_ARM ARM64)
add_subdirectory(asmjit)
target_link_libraries(${CMAKE_PROJECT_NAME} asmjit)

##### SDL #########################
set(SDL_SHARED OFF)
set(SDL_STATIC ON)
set(SDL_TEST_LIBRARY OFF)
set(SDL_TESTS OFF)
set(SDL_SHARED_ENABLED_BY_DEFAULT OFF)
set(SDL_STATIC_ENABLED_BY_DEFAULT ON)
set(SDL_TEST_ENABLED_BY_DEFAULT OFF)
set(SDL2_DISABLE_SDL2MAIN ON)
set(SDL2_DISABLE_INSTALL ON)
set(SDL2_DISABLE_UNINSTALL ON)
add_subdirectory(SDL)
target_compile_options(SDL3-static PRIVATE -w) # disable warnings
target_link_libraries(${CMAKE_PROJECT_NAME} SDL3-static)

##### imgui #########################
add_library(imgui STATIC
	imgui/imgui.cpp
	imgui/imgui_draw.cpp
	imgui/imgui_tables.cpp
	imgui/imgui_widgets.cpp
	imgui/backends/imgui_impl_sdl3.cpp
)
target_include_directories(imgui PRIVATE imgui)
target_link_libraries(imgui SDL3-static)
target_link_libraries(${CMAKE_PROJECT_NAME} imgui)
