#ifndef _EVENT_MANAGER_EVENTS_H
#define _EVENT_MANAGER_EVENTS_H
#include <pthread.h>
#include <stdint.h>

#ifndef MAX_EVENT_ARG_SIZE
#warning "Event argument max size is set to default 128"
#define MAX_EVENT_ARG_SIZE  128
#endif

typedef struct
{
    uint32_t id;
    uint8_t args[MAX_EVENT_ARG_SIZE - sizeof(uint32_t)];
} events_t;

#endif
