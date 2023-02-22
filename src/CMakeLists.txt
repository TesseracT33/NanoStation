cmake_minimum_required (VERSION 3.13)

target_sources(NanoStation PRIVATE
	main.cpp

	common/emulator.cpp
	common/log.cpp
	common/scheduler.cpp

	ee/cop0.cpp
	ee/cop1.cpp
	ee/cop2.cpp
	ee/cpu.cpp
	ee/ee.cpp
	ee/exceptions.cpp
	ee/mmi.cpp
	ee/mmu.cpp
	ee/timers.cpp

	frontend/message.cpp

	iop/cop0.cpp
	iop/cop2.cpp
	iop/cpu.cpp
	iop/exceptions.cpp
	iop/iop.cpp
	iop/memory.cpp

	mips/disassembler.cpp
 )

target_include_directories(NanoStation PRIVATE
	.
	common
)