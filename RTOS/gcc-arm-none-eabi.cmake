set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Default GCC settings
set(TOOLCHAIN_PREFIX arm-none-eabi-)

set(ASM_FLAGS "-c -x assembler-with-cpp --specs=nano.specs")
set(C_FLAGS "-ffunction-sections -fdata-sections -fstack-usage --specs=nano.specs")
set(CXX_FLAGS "-fno-exceptions -fno-rtti -fno-use-cxa-atexit")

set(CMAKE_ASM_COMPILER ${TOOLCHAIN_PREFIX}gcc ${ASM_FLAGS})
set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}gcc ${C_FLAGS})
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++ ${C_FLAGS} ${CXX_FLAGS})
set(CMAKE_OBJCOPY ${TOOLCHAIN_PREFIX}objcopy)
set(CMAKE_SIZE ${TOOLCHAIN_PREFIX}size)

set(CMAKE_EXECUTABLE_SUFFIX_ASM ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX ".elf")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
