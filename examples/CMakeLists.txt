# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name and version
project(app VERSION 1.0)

# Set the default build type to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif()

# Option to use dynamic memory
option(USE_DYNAMIC_MEM "Use dynamic memory allocation" OFF)

# Option to use static memory
option(USE_STATIC_MEM "Use static memory allocation" ON)

# Specify the C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

# Compiler flags for Debug build (adds debug symbols)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")


add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../event_manager EventManager)

# Add the executable (main.c is the source file)
add_executable(app 
            two_tasks_subscribe.c)

target_link_libraries(app EventManager)