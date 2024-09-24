add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/posix/queue)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/posix/tasks)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/events)

if (NOT USE_DYNAMIC_MEM AND NOT USE_STATIC_MEM)
    message(FATAL_ERROR "Error: Either USE_DYNAMIC_MEM or USE_STATIC_MEM must be enabled.")
endif()

if (USE_DYNAMIC_MEM)
    add_library(EventManager src/event_manager_dynamic.c)
endif()

if (USE_STATIC_MEM)
    add_library(EventManager src/event_manager_static.c)
endif()

target_include_directories(EventManager 
                            PUBLIC
                            ${CMAKE_CURRENT_SOURCE_DIR}/inc)

target_link_libraries(EventManager PUBLIC queue task events)