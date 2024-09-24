add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/posix/queue)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/posix/tasks)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/events)

add_library(EventManager src/event_manager.c)

target_include_directories(EventManager 
                            PUBLIC
                            ${CMAKE_CURRENT_SOURCE_DIR}/inc)

target_link_libraries(EventManager PUBLIC queue task events)