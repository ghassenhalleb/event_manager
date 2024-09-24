add_library(queue ${CMAKE_CURRENT_SOURCE_DIR}/src/event_manager_queue.c)


target_include_directories(queue PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
