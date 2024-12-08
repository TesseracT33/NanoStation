cmake_minimum_required(VERSION 3.13)

project(NanoStation LANGUAGES CXX C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(NANOSTATION_LIB ${CMAKE_PROJECT_NAME}Lib)

if (CMAKE_SYSTEM_PROCESSOR MATCHES "AMD64" OR CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
	set(X64 1)
elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "ARM64" OR CMAKE_SYSTEM_PROCESSOR MATCHES "arm64" 
		OR CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
	set(ARM64 1)
else()
    message(FATAL_ERROR "Unsupported architecture \"${CMAKE_SYSTEM_PROCESSOR}\"; only x86-64 and arm64 are supported.")
endif()

include(GlobalCompilerOptions.cmake)

add_library(${NANOSTATION_LIB} STATIC)

add_subdirectory(ext)

include(CompilerOptions.cmake)

add_subdirectory(src)

add_executable(${CMAKE_PROJECT_NAME}
	src/main.cpp
)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
	src/
	src/common/
)

target_link_libraries(${CMAKE_PROJECT_NAME}
	${NANOSTATION_LIB}
)

add_subdirectory(test)
