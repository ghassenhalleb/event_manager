#ifndef _EVENT_MANAGER_EVENTS_H
#define _EVENT_MANAGER_EVENTS_H
#include <pthread.h>
#include <stdint.h>

#define MAX_EVENT_ARG_SIZE  128

typedef struct
{
    uint32_t id;
    uint8_t args[MAX_EVENT_ARG_SIZE];
} events_t;

#endif
