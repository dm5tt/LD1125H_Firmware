# Set system and compiler settings
cmake_minimum_required(VERSION 3.22)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)

# Project configuration
project(CLionGD32F303CG LANGUAGES C CXX ASM)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

# Compiler definitions
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_AR arm-none-eabi-ar)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)
set(SIZE arm-none-eabi-size)

# Try compile target type
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Set the linker script
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/Shared/gd3f303cet6.ld)

# Define project options
add_compile_options(
    -mcpu=cortex-m4
    -mthumb
    -mthumb-interwork
    -mfloat-abi=hard
    -ffunction-sections
    -fdata-sections
    -fno-common
    -fmessage-length=0
    -Og
    -g
    -Wall
    -Wextra
)

# Specific ASM compile option
add_compile_options($<$<COMPILE_LANGUAGE:ASM>:-x$<SEMICOLON>assembler-with-cpp>)

# Include directories
include_directories(
    Include
    Drivers/FreeRTOS/include
    Drivers/FreeRTOS/portable/GCC/ARM_CM4F
    Drivers/GD32F30x_standard_peripheral/Include
    Drivers/CMSIS/
    Drivers/CMSIS/GD/GD32F30x/Include
    Drivers/GD32F30x_usbd_library/class/device/cdc/Include
    Drivers/GD32F30x_usbd_library/device/Include
    Drivers/GD32F30x_usbd_library/usbd/Include
)

# Preprocessor definitions
add_definitions(-DDEBUG -DGD32F3 -DGD32F30x -DGD32F303 -DGD32F30X_HD)

# Linker options
set(LINKER_OPTIONS
    -Wl,-gc-sections
    -Wl,--print-memory-usage
    -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map
    -mcpu=cortex-m4
    -mthumb
    -mthumb-interwork
    --specs=rdimon.specs -lc -lrdimon
    -mfloat-abi=hard
    -T ${LINKER_SCRIPT}
)

function(add_app app_name)
    # Set source files for the specific app
    file(GLOB_RECURSE APP_SOURCES
        "${CMAKE_SOURCE_DIR}/Apps/${app_name}/*.*"
        "${CMAKE_SOURCE_DIR}/Source/*.*"
        "${CMAKE_SOURCE_DIR}/Drivers/*.*"
    )

    # Include directories for the specific app
    file(GLOB_RECURSE APP_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/Apps/${app_name}/")
    list(REMOVE_DUPLICATES APP_INCLUDE_DIRS)

    # Define the executable target for the app without .elf suffix
    add_executable(${app_name} ${APP_SOURCES})

    # Add include directories to the target
    target_include_directories(${app_name} PRIVATE ${APP_INCLUDE_DIRS})

    # Linker options
    target_link_options(${app_name} PRIVATE ${LINKER_OPTIONS})

    # Output files (HEX and BIN)
    set(HEX_FILE ${PROJECT_BINARY_DIR}/${app_name}.hex)
    set(BIN_FILE ${PROJECT_BINARY_DIR}/${app_name}.bin)

    # Post-build commands
    add_custom_command(TARGET ${app_name} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${app_name}> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${app_name}> ${BIN_FILE}
        COMMENT "Generating ${HEX_FILE} and ${BIN_FILE}"
    )

    set_target_properties(${app_name} PROPERTIES
        OUTPUT_NAME ${app_name}
        SUFFIX ".elf"
    )
endfunction()

# List of app targets
set(APP_LIST
   hello_world
   cw_radar
    # Add more app names here
)

# Add each app target
foreach(app ${APP_LIST})
    add_app(${app})
endforeach()
