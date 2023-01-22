cmake_minimum_required (VERSION 3.8)

add_executable(NanoStation
	main.cpp

	common/emulator.cpp

	ee/cop0.cpp
	ee/cop1.cpp
	ee/cop2.cpp
	ee/cpu_interpreter.cpp
	ee/disassembler.cpp
	ee/ee.cpp
	ee/exceptions.cpp
	ee/mmi.cpp
)

target_include_directories(NanoStation PRIVATE
	.
	common
)