#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include "event_manager.h"

typedef struct subscriber_node
{
    task_context_t *task;
    struct subscriber_node *next;
} subscriber_node_t;

typedef struct event_subscribers_node
{
    uint32_t event_id;
    subscriber_node_t *subscribers_head;
    struct event_subscribers_node *next;
} event_subscribers_node_t;

typedef struct 
{
    event_subscribers_node_t *head;
} events_subscribers_t;

static pthread_t event_manager_thread;
static queue_t *event_manager_queue = NULL;
static uint32_t event_manager_queue_receive_timeout;
static events_subscribers_t events_list = {NULL};

static bool is_subsribed(uint32_t event_id, task_context_t *task);
static event_subscribers_node_t *event_manager_find_event(uint32_t event_id);
static event_subscribers_node_t *create_event_node(uint32_t event_id);
static subscriber_node_t *create_subscriber_node(task_context_t *task);
static void *event_manager_task(void *arg);

static event_subscribers_node_t *event_manager_find_event(uint32_t event_id)
{
    event_subscribers_node_t *current = events_list.head;
    while (current != NULL)
    {
        if (current->event_id == event_id)
        {
            return current;
        }
        current = current->next;
    }
    return NULL;
}
static bool is_subsribed(uint32_t event_id, task_context_t *task)
{
    event_subscribers_node_t *event = event_manager_find_event(event_id);
    
    subscriber_node_t *current = event->subscribers_head;

    while (current != NULL)
    {
        if (current->task == task)
        {
            return true;
        }

        current = current->next;
    }

    return false;
}

static event_subscribers_node_t *create_event_node(uint32_t event_id)
{
    event_subscribers_node_t *new_node = (event_subscribers_node_t *)malloc(sizeof(event_subscribers_node_t));
    if (!new_node)
    {
        return NULL;
    }

    new_node->event_id = event_id;
    new_node->subscribers_head = NULL;
    new_node->next = events_list.head;
    events_list.head = new_node;

    return new_node;
}

static subscriber_node_t *create_subscriber_node(task_context_t *task)
{
    subscriber_node_t *new_node = (subscriber_node_t *)malloc(sizeof(subscriber_node_t));
    if (!new_node)
    {
        exit(EXIT_FAILURE);
    }

    new_node->task = task;
    new_node->next = NULL;

    return new_node;
}

int event_manager_init(uint32_t receive_timeout)
{
    event_manager_queue = queue_init(sizeof(events_t));
    if (event_manager_queue == NULL)
    {
        return -1;
    }
    event_manager_queue_receive_timeout = receive_timeout;

    if (pthread_create(&event_manager_thread, NULL, event_manager_task, NULL) < 0)
    {
        return -1;
    }

    return 0;
}

int event_manager_register_event(uint32_t event_id)
{
    int ret = -1;
    if (event_manager_find_event(event_id) == NULL)
    {
        if (create_event_node(event_id) != NULL)
        {
            ret = 0;
        }
    }
    return ret;
}

int event_manager_subscribe_event(uint32_t event_id, task_context_t *task)
{
    event_subscribers_node_t *event_node = event_manager_find_event(event_id);
    if (event_node == NULL)
    {
        return -1;
    }

    /* Already subscribed */
    if (is_subsribed(event_id, task) == true)
    {
        return -2;
    }

    subscriber_node_t *new_subscriber = create_subscriber_node(task);
    new_subscriber->next = event_node->subscribers_head;
    event_node->subscribers_head = new_subscriber;

    return 0;
}

int event_manager_unsubscribe_event(uint32_t event_id, task_context_t *task)
{
    event_subscribers_node_t *event_node = event_manager_find_event(event_id);
    if (event_node == NULL)
    {
        return -1;
    }

    subscriber_node_t *current = event_node->subscribers_head;
    subscriber_node_t *previous = NULL;

    while (current != NULL)
    {
        if (current->task == task)
        {
            if (previous == NULL)
            {
                event_node->subscribers_head = current->next;
            }
            else
            {
                previous->next = current->next;
            }

            free(current);
            return 0;
        }

        previous = current;
        current = current->next;
    }

    return -1;
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
    event_subscribers_node_t *event_node;
    subscriber_node_t *subscriber;

    while (1)
    {
        event = queue_receive(event_manager_queue, event_manager_queue_receive_timeout);
        if (event)
        {
            event_node = event_manager_find_event(event->id);
            if (event_node != NULL)
            {
                subscriber = event_node->subscribers_head;
                while (subscriber != NULL)
                {
                    queue_send(subscriber->task->input_queue, event);
                    subscriber = subscriber->next;
                }
            }
        }
    }
}
