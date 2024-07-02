project(GameBoy C)
# set(CMAKE_C_COMPILER "gcc")
message("MINEMU GameBoy")

# List all the source files of the GameBoy library
set(GB_HEADERS
    include/GameBoy.h
    include/SOC/GB_Bus.h
    include/SOC/GB_Instructions.h
    include/SOC/GB_Registers.h
    include/Emulation/GB_Emulation.h
    include/Emulation/GB_Instruction.h
    include/Emulation/GB_SystemContext.h
    include/Memory/GB_MemoryMap.h
    include/Memory/GB_Header.h
    include/PPU/GB_Pallete.h
)

set(GB_SOURCES
    src/SOC/GB_Instructions.c
    src/SOC/GB_Bus.c
    src/Emulation/GB_Emulation.c
)

# Create the GameBoy_MINEMU shared library
add_library(GameBoy STATIC  ${GB_SOURCES} ${GB_HEADERS})

if(MINEMU_DEBUG)
    add_compile_definitions(GB_DEBUG)
endif()

# Include directories for the proyect itself...
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Include directories for GameBoy
target_include_directories(GameBoy PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link any necessary libraries 
target_link_libraries(GameBoy PRIVATE Core)

