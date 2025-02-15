cmake_minimum_required (VERSION 3.13)

target_sources(${NANOSTATION_LIB} PRIVATE
	common/emulator.cpp
	common/jit_common.cpp
	common/scheduler.cpp

	ee/cop0.cpp
	ee/cop1.cpp
	ee/cop2.cpp
	ee/cpu.cpp
	ee/ee.cpp
	ee/exceptions.cpp
	ee/intc.cpp
	ee/jit.cpp
	ee/mmi.cpp
	ee/mmu.cpp
	ee/register_allocator.cpp
	ee/timers.cpp
	ee/vu_interpreter.cpp

	frontend/message.cpp

	iop/cop0.cpp
	iop/cop2.cpp
	iop/cpu.cpp
	iop/exceptions.cpp
	iop/iop.cpp
	iop/memory.cpp

	mips/decoder.cpp
)

target_include_directories(${NANOSTATION_LIB} PUBLIC
	.
	common
)
