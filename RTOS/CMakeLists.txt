cmake_minimum_required(VERSION 3.12)

project(RTOS)

set(PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/App)

# Configure preprocessor symbols need to use the HAL library and specifie waht HAL MCU.
# -------------------------------------------------------------------------------------
set(MCU_FAMILY STM32F4xx)
set(MCU_MODEL STM32F429xx)
set(HAL_DRIVER USE_HAL_DRIVER)
# -------------------------------------------------------------------------------------

# ---------------- Flags used to setup parameters of specific hardware ----------------
set(MCU_PARAMETERS -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb)
# -------------------------------------------------------------------------------------

# ----------- Setting sript paths necessary for linker and startup process. -----------
set(MCU_STARTUP_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/Core/Startup/startup_stm32f429zitx.s)
set(MCU_LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/STM32F429ZITX_FLASH.ld)
# -------------------------------------------------------------------------------------

enable_language(C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# ------------------------- Configure the header folder paths -------------------------
set(HAL_INCLUDE_DERECTORIES
   ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Include
   ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Device/ST/${MCU_FAMILY}/Include
   ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/${MCU_FAMILY}_HAL_Driver/Inc
   ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/${MCU_FAMILY}_HAL_Driver/Inc/Legacy)

set(CORE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc)

set(PROJECT_INCLUDE_DIRECTORIES ${PROJECT_DIR}/Inc)
# -------------------------------------------------------------------------------------

# ------------------------- Configure the source folder paths -------------------------
file(GLOB_RECURSE HAL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/*.c)

file(GLOB_RECURSE CORE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Core/*.c)

file(GLOB_RECURSE APP_SOURCES FOLLOW_SYMLINKS
    ${PROJECT_DIR}/*.c
    ${PROJECT_DIR}/*.cpp)
# -------------------------------------------------------------------------------------

add_executable(${CMAKE_PROJECT_NAME} ${HAL_SOURCES} ${CORE_SOURCES} ${MCU_STARTUP_SCRIPT})

# ----------------------------- Embedded Macros (defile). -----------------------------
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    ${MCU_MODEL}
    ${HAL_DRIVER})
# -------------------------------------------------------------------------------------

# --------------------------------- Header directories --------------------------------
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${HAL_INCLUDE_DERECTORIES}
    ${CORE_INCLUDE_DIRECTORIES}
    ${PROJECT_INCLUDE_DIRECTORIES})
# -------------------------------------------------------------------------------------


# ---------------------------- Compiler and linker options ----------------------------
target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE
    ${MCU_PARAMETERS}
    -Wall
    -Wextra
    -Wpedantic
    -Wno-unused-parameter
    $<$<COMPILE_LANGUAGE:CXX>:
        -Wno-volatile
        -Wold-style-cast
        -Wuseless-cast
        -Wsuggest-override>
    $<$<CONFIG:Debug>:-O0 -g3>
    $<$<CONFIG:Release>:-Os -g0>)

target_link_options(${CMAKE_PROJECT_NAME} PRIVATE
    -T${MCU_LINKER_SCRIPT}
    ${MCU_PARAMETERS}
    --specs=nosys.specs
    -Wl,-Map=${CMAKE_PROJECT_NAME}.map
    -Wl,--gc-sections
    -static
    -Wl,--start-group
    -lc
    -lm
    -lstdc++
    -lsupc++
    -Wl,--end-group
    -Wl,--print-memory-usage)
# -------------------------------------------------------------------------------------

add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${CMAKE_PROJECT_NAME}>)

add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${CMAKE_PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${CMAKE_PROJECT_NAME}.bin)


