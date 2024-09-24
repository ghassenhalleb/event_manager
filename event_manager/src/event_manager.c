#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "event_manager.h"
#include "event_manager_queue.h"
#include "event_manager_events.h"


static pthread_t event_manager_thread;
static uint32_t event_manager_queue_receive_timeout;
queue_t *event_manager_queue = NULL;

/* Linked list for event subscribers */
typedef struct subscriber_node
{
    task_context_t *task;  // Subscriber task
    struct subscriber_node *next; // Pointer to the next subscriber
} subscriber_node_t;

/* Linked list for events */
typedef struct event_subscribers_node
{
    uint32_t event_id;                      // Event ID
    subscriber_node_t *subscribers_head;    // Head of the subscriber linked list
    struct event_subscribers_node *next;    // Pointer to the next event node
} event_subscribers_node_t;

/* Event manager structure with head of the event list */
typedef struct 
{
    event_subscribers_node_t *head;  // Head of the event linked list
} events_subscribers_t;

static events_subscribers_t events_list = {NULL};  // Initialize the event list as empty

/* Function prototypes */
static void *event_manager_task(void *arg);
static event_subscribers_node_t *event_manager_find_event(uint32_t event_id);
static event_subscribers_node_t *create_event_node(uint32_t event_id);
static subscriber_node_t *create_subscriber_node(task_context_t *task);

/* Find an event node by event ID in the linked list */
static event_subscribers_node_t *event_manager_find_event(uint32_t event_id)
{
    event_subscribers_node_t *current = events_list.head;
    while (current != NULL)
    {
        if (current->event_id == event_id)
        {
            return current;  // Event found
        }
        current = current->next;
    }
    return NULL;  // Event not found
}

/* Create a new event node and add it to the linked list */
static event_subscribers_node_t *create_event_node(uint32_t event_id)
{
    // Allocate memory for the new event node
    event_subscribers_node_t *new_node = (event_subscribers_node_t *)malloc(sizeof(event_subscribers_node_t));
    if (!new_node)
    {
        return NULL;
    }

    // Initialize the new event node
    new_node->event_id = event_id;
    new_node->subscribers_head = NULL;  // No subscribers initially
    new_node->next = NULL;

    // Insert the new node at the head of the events list
    new_node->next = events_list.head;
    events_list.head = new_node;

    return new_node;
}

/* Create a new subscriber node for a task */
static subscriber_node_t *create_subscriber_node(task_context_t *task)
{
    // Allocate memory for the new subscriber node
    subscriber_node_t *new_node = (subscriber_node_t *)malloc(sizeof(subscriber_node_t));
    
    if (!new_node)
    {
        exit(EXIT_FAILURE);
    }

    // Initialize the subscriber node
    new_node->task = task;
    new_node->next = NULL;

    return new_node;
}

/* Initialize the event manager */
int event_manager_init(uint32_t receive_timeout)
{
    event_manager_queue = queue_init(sizeof(events_t));
    if (event_manager_queue == NULL)
    {
        return -1;
    }
    /* Initialize the queue receive timeout */
    event_manager_queue_receive_timeout = receive_timeout;

    if (pthread_create(&event_manager_thread, NULL, event_manager_task, "event manager task") < 0)
    {
        return -1;
    }

    return 0;
}

/* Register a new event and add it to the linked list */
int event_manager_register_event(uint32_t event_id)
{
    int ret = -1;
    // Check if the event is already registered
    if (event_manager_find_event(event_id) == NULL)
    {
        // Create a new event node and add it to the list
        if (create_event_node(event_id) != NULL)
        {
            ret = 0;
        }
    }
    return ret;  // Event already exists
}

/* Publish an event to the event manager queue */
int event_manager_publish_event(events_t *event)
{
    return queue_send(event_manager_queue, event);
}

/* Subscribe a task to an event */
int event_manager_subscribe_event(uint32_t event_id, task_context_t *task)
{
    // Find the event node by ID
    event_subscribers_node_t *event_node = event_manager_find_event(event_id);
    if (event_node == NULL)
    {
        return -1;  // Event not found
    }

    // Create a new subscriber node
    subscriber_node_t *new_subscriber = create_subscriber_node(task);

    // Add the subscriber to the event's linked list of subscribers
    new_subscriber->next = event_node->subscribers_head;
    event_node->subscribers_head = new_subscriber;

    return 0;
}

/* Unsubscribe a task from an event */
int event_manager_unsubscribe_event(uint32_t event_id, task_context_t *task)
{
    // Find the event node by ID
    event_subscribers_node_t *event_node = event_manager_find_event(event_id);
    if (event_node == NULL)
    {
        return -1;  // Event not found
    }

    /* The element exist now remove it */
    subscriber_node_t *current = event_node->subscribers_head;
    subscriber_node_t *previous = NULL;

    while (current != NULL)
    {
        if (current->task == task)  // Found the task to unsubscribe
        {
            if (previous == NULL)
            {
                // The subscriber to remove is the first node
                event_node->subscribers_head = current->next;
            }
            else
            {
                // The subscriber is somewhere in the middle or end
                previous->next = current->next;
            }

            // Free the memory for the unsubscribed node
            free(current);
            return 0;  // Successfully unsubscribed
        }

        // Move to the next node
        previous = current;
        current = current->next;
    }

    return -1;  // Subscriber not found
}

/* The event manager task that dispatches events to subscribers */
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
            // Find the event node
            event_node = event_manager_find_event(event->id);
            if (event_node != NULL)
            {
                // Dispatch the event to all subscribers in the linked list
                subscriber = event_node->subscribers_head;
                while (subscriber != NULL)
                {
                    queue_send(subscriber->task->input_queue, event);
                    subscriber = subscriber->next;  // Move to the next subscriber
                }
            }
        }
    }
}
