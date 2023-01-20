cmake_minimum_required (VERSION 3.8)

add_executable(NanoStation
	main.cpp

	ee/cpu_interpreter.cpp
	ee/disassembler.cpp
	ee/ee.cpp
	ee/exceptions.cpp
)

target_include_directories(NanoStation PRIVATE
	.
	common
)