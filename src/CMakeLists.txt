cmake_minimum_required (VERSION 3.8)

add_executable(NanoStation
	main.cpp

	ee/disassembler.cpp
	ee/ee.cpp
	ee/ee_interpreter.cpp
)

target_include_directories(NanoStation PRIVATE
	common
	ee
)