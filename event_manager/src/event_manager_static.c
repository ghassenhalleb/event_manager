#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "event_manager.h"

#define EVENTS_COUNT    20
#define MAX_EVENT_SUBSCRIBERS 10

typedef struct 
{
    uint32_t event_id;
    task_context_t *subscribers[MAX_EVENT_SUBSCRIBERS];
    uint32_t event_subscribers_count;
} event_subscribers_t;

static pthread_t event_manager_thread;
static queue_t *event_manager_queue = NULL;

static event_subscribers_t events_list[EVENTS_COUNT];
static uint32_t event_count = 0;

static void *event_manager_task(void *arg);


static int event_manager_find_event_id(uint32_t event_id)
{
    for (uint32_t i = 0; i < event_count; i++)
    {
        if (events_list[i].event_id == event_id)
        {
            return i;
        }
    }
    return -1;
}

int event_manager_init(uint32_t receive_timeout)
{
    event_manager_queue = queue_init(sizeof(events_t));
    if (event_manager_queue == NULL)
    {
        return -1;
    }

    if (pthread_create(&event_manager_thread, NULL, event_manager_task, NULL) < 0)
    {
        return -1;
    }

    return 0;
}

int event_manager_register_event(uint32_t event_id)
{
    int ret = -1;
    if (event_manager_find_event_id(event_id) == -1)
    {
        if (event_count < EVENTS_COUNT)
        {
            events_list[event_count].event_id = event_id;
            events_list[event_count].event_subscribers_count = 0;
            event_count++;
            ret = 0;
        }
    }
    return ret;
}

int event_manager_subscribe_event(uint32_t event_id, task_context_t *task)
{
    int ret = -1;
    int event_index = event_manager_find_event_id(event_id);

    if (event_index != -1)
    {
        event_subscribers_t *event = &events_list[event_index];
        if (event->event_subscribers_count < MAX_EVENT_SUBSCRIBERS)
        {
            event->subscribers[event->event_subscribers_count] = task;
            event->event_subscribers_count++;
            ret = 0;
        }
    }
    return ret;
}

int event_manager_unsubscribe_event(uint32_t event_id, task_context_t *task)
{
    int ret = -1;
    int event_index = event_manager_find_event_id(event_id);

    if (event_index != -1)
    {
        event_subscribers_t *event = &events_list[event_index];
        uint32_t i;
        for (i = 0; i < event->event_subscribers_count; i++)
        {
            if (event->subscribers[i] == task)
            {
                break;
            }
        }

        if (i < event->event_subscribers_count)
        {
            for (; i < event->event_subscribers_count - 1; i++)
            {
                event->subscribers[i] = event->subscribers[i + 1];
            }
            event->event_subscribers_count--;
            ret = 0;
        }
    }
    return ret;
}

int event_manager_publish_event(events_t *event)
{
    return queue_send(event_manager_queue, event);
}

events_t * event_manager_wait_for_events(task_context_t *task, uint32_t timeout)
{
    if (task->input_queue == NULL)
    {
        return NULL;
    }
    return (events_t *)queue_receive(task->input_queue, timeout);
}

static void *event_manager_task(void *arg)
{
    events_t *event;
    int event_index;

    while (1)
    {
        event = queue_receive(event_manager_queue, 1000);
        if (event)
        {
            event_index = event_manager_find_event_id(event->id);
            if (event_index != -1)
            {
                event_subscribers_t *event_subscribers = &events_list[event_index];
                for (uint32_t i = 0; i < event_subscribers->event_subscribers_count; i++)
                {
                    queue_send(event_subscribers->subscribers[i]->input_queue, event);
                }
            }
        }
    }
}
