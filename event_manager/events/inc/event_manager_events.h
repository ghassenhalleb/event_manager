#ifndef _EVENT_MANAGER_EVENTS_H
#define _EVENT_MANAGER_EVENTS_H
#include <pthread.h>
#include <stdint.h>

typedef struct
{
    uint32_t id;
    void *args;
} events_t;

#endif