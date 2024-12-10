#include <stdio.h>
#include <pthread.h>
#include "event_manager_queue.h"
#include "event_manager_tasks.h"
#include "event_manager_events.h"


int event_manager_task_create(void *(*routine)(void *))
{
    // Dynamically allocate memory for the task structure
    task_context_t *arg = (task_context_t *)malloc(sizeof(task_context_t));
    if (arg == NULL)
    {
        return -1;
    }

    
    // Initialize the input queue
    arg->input_queue = queue_init(sizeof(events_t));
    if (arg->input_queue == NULL)
    {
        free(arg);  // Free memory if queue initialization fails
        return -1;
    }

    // Pass the pointer to the task structure to the pthread_create
    if (pthread_create(&arg->task_handler, NULL, routine, (void *)arg) < 0)
    {
        free(arg);  // Free memory if thread creation fails
        return -1;
    }

    return 0;
}