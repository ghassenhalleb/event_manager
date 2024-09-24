#include <stdio.h>
#include <pthread.h>
#include <time.h>

#include "event_manager_events.h"
#include "event_manager_tasks.h"
#include "event_manager_queue.h"
#include "event_manager.h"


static void *task1(void *arg);
static void *task2(void *arg);

static void *task1(void *arg)
{
    task_context_t *this = (task_context_t *)arg;
    events_t *event;
    event_manager_subscribe_event(2, this);
    while(1)
    {
        if (this->input_queue)
        {
            event = queue_receive(this->input_queue, 1000);
            if (event)
            {
                printf("event received in task1 %d\n\r", event->id);
                event_manager_unsubscribe_event(2, this);
            }
        }
    }
}

static void *task2(void *arg)
{
    task_context_t *this = (task_context_t *)arg;
    events_t *event;
    event_manager_subscribe_event(2, this);
    while(1)
    {
        if (this->input_queue)
        {
            event = queue_receive(this->input_queue, 1000);
            if (event)
            {
                printf("event received in task2 %d\n\r", event->id);
            }
        }
    }
}

void main(void)
{
    printf("Hello world \n\r");
    if (event_manager_init(1000) != 0)
    {
        printf("Error Handler init\r\n");
    }
    /* Register the event inside the list of events */
    event_manager_register_event(2);
    event_manager_task_create(task1);
    event_manager_task_create(task2);
    events_t event;
    uint16_t event_id = 0;
    while(1)
    {
        event.id = event_id;
        event_manager_publish_event(&event);
        printf("Main Loop event %d\n\r", event_id);
        event_id ++;
        event_id = event_id % 5;
        usleep(1000 * 1000);
    }
}