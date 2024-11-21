#ifndef _POSIX_EVENT_MANAGER_TASKS_H
#define _POSIX_EVENT_MANAGER_TASKS_H
#include "event_manager_queue.h"
#include "pthread.h"

typedef struct
{
    pthread_t task_handler;
    queue_t *input_queue;
} task_context_t;


int event_manager_task_create(void *(*routine)(void *));



#endif