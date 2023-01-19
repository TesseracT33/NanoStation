cmake_minimum_required(VERSION 3.8)

project(NanoStation LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(src)

if (CMAKE_VERSION VERSION_GREATER 3.12)
	set_property(TARGET NanoStation PROPERTY CXX_STANDARD 23 CXX_STANDARD_REQUIRED ON)
endif()

if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "AMD64" OR ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86-64")
	set(X64 1)
endif()

if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "ARM64" OR ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "aarch64")
	set(ARM64 1)
endif()

if (NOT X64 AND NOT ARM64)
	message(FATAL_ERROR "Unsupported architecture; only x86-64 and arm64 are supported.")
endif()

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	set(DEBUG 1)
endif()

if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
	set(RELEASE 1)
endif()

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
	set(CLANG 1)
endif()

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
	set(GCC 1)
endif()

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
	set(MSVC 1)
endif()

list(APPEND clang_flags
	-Wall
	-Wconversion
	-Wextra
	-Wno-bitwise-op-parentheses
	-Wno-sign-conversion
	-Wpedantic
)

if (DEBUG)
	list(APPEND clang_flags
		-Og
	)
else()
	list(APPEND clang_flags
		-flto
		-O3
	)
endif()

if (X64)
	list(APPEND clang_flags
		-mavx2
		-arch:AVX2
	)
endif()

list(APPEND gcc_flags
	-std=c++23
	-Wall
	-Wconversion
	-Wextra
	-Wno-bitwise-op-parentheses
	-Wno-sign-conversion
	-Wpedantic
)

if (DEBUG)
	list(APPEND gcc_flags
		-Og
	)
else()
	list(APPEND gcc_flags
		-flto
		-O3
	)
endif()

if (X64)
	list(APPEND gcc_flags
		-mavx2
	)
endif()

list(APPEND msvc_flags
	/fp:except     # Allows proper reading of floating-point exceptions
	/fp:strict     # Strict floating-point model
	/GR            # Enable run-time type information
	/MP            # Build with multiple processes
	/permissive-   # Disallow non-standard behaviour
	/sdl           # Enable Additional Security Checks
	/std:c++latest # C++ standard
	/volatile:iso  # Use iso standard for 'volatile'
	/W4            # Warning level 4
)

if (DEBUG)
	list(APPEND msvc_flags
		/GS  # Buffer security check
		/JMC # Just My Code debugging
		/MDd # Use run-time library MSVCRTD.lib
		/Od  # Disable (Debug)
		/Zi  # Debug information format; supporting the 'Edit and Continue' feature of VS
	)
else()
	list(APPEND msvc_flags
		/GL # Whole program optimization
		/MD # Use run-time library MSVCRT.lib
		/O2 # Maximize speed
		/Oi # Generate intrinsic functions
	)
endif()


if (X64)
	list(APPEND msvc_flags
		/arch:AVX2
	)
endif()

# Correctly replace default /W3 with /W4
if (MSVC)
	string(REGEX REPLACE "/W[1-3]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()

if (CLANG)
	target_compile_options(NanoStation PUBLIC ${clang_flags})
elseif (GCC)
	target_compile_options(NanoStation PUBLIC ${gcc_flags})
elseif (MSVC)
	target_compile_options(NanoStation PUBLIC ${msvc_flags})
endif()