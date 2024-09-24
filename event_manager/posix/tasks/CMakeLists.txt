add_library(task ${CMAKE_CURRENT_SOURCE_DIR}/src/event_manager_tasks.c)


target_include_directories(task 
                            PUBLIC 
                            ${CMAKE_CURRENT_SOURCE_DIR}/inc)

target_link_libraries(task PRIVATE queue events)