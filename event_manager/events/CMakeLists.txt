add_library(events src/event_manager_events.c)

target_include_directories(events 
                            PUBLIC
                            ${CMAKE_CURRENT_SOURCE_DIR}/inc)
