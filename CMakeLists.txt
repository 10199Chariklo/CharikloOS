cmake_minimum_required(VERSION 3.16)
project(CharikloOS C)

# Enforce strict standard C11 compilation guidelines across Clang
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Configure output directories for compiled binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Automate discovery of header inclusion zones
include_directories(include)
include_directories(src)

# --------------------------------------------------------------------------
# PHYSICSFS LINKAGE INTERCEPT
# --------------------------------------------------------------------------
#physicsfs is just the entire PhysicsFS git repo src file
#Rawdog "BUILD IT WITH YOUR OWN PROGRAM" style
include_directories(physicsfs)
file(GLOB PHYSFS_SOURCES "physicsfs/*.c")

# --------------------------------------------------------------------------
# SYSTEM CORE KERNEL CHASSIS TARGET
# --------------------------------------------------------------------------
# Gather all system engine files living in our directories and ANY sub-directories
file(GLOB_RECURSE CORE_SOURCES "src/*.c")

# Now we set all our gathered directories into a single entry; KERNEL_SOURCES
set(KERNEL_SOURCES ${CORE_SOURCES} ${PHYSFS_SOURCES})

# --------------------------------------------------------------------------
# INDEPENDENT UNIT TEST TARGETS
# --------------------------------------------------------------------------
# Register our standalone PhysFS Playground binary execution file
add_executable(RevisionTest04 
    unittest/RevisionTest04 
    ${KERNEL_SOURCES}
)

# Windows Subsystem Patch: If compiling on a host running Win32 Clang,
# explicitly direct the linker to map console outputs safely.
# Our setup routine begines as a WinAPI GUI application - So we can safely call AllocConsole and style a console with impunity
if(WIN32)
    target_link_libraries(RevisionTest04 PRIVATE "-mwindows")
endif()
