
# Event Manager

This project implements an event management system using a linked list-based approach to manage events and their subscribers. The event manager allows tasks to register events, subscribe to events, and publish events. The system is implemented using threads and queues to handle asynchronous event dispatching.

## Features

- **Event Registration**: Dynamically register events by their `event_id`.
- **Event Subscription**: Subscribe tasks to specific events.
- **Event Publishing**: Publish events to be dispatched to all subscribed tasks.
- **Event Unsubscription**: Remove tasks from an event's subscriber list.
- **Event Unregistration**: Remove events and all their associated subscribers from the system.
- **Queue-based Asynchronous Event Dispatch**: Events are published to a queue and processed by an event manager task, which dispatches them to the relevant subscribers.

## Data Structures

### Event Linked List

The event manager maintains a linked list of events. Each event is represented by a node (`event_subscribers_node_t`), and each node has its own linked list of subscribers (`subscriber_node_t`).

### Event Node Structure

- `event_id`: The unique identifier for the event.
- `subscribers_head`: The head of the linked list of subscribers.
- `next`: Pointer to the next event in the list.

### Subscriber Node Structure

- `task`: The task that subscribes to the event.
- `next`: Pointer to the next subscriber in the list.

## Code Overview

### \`event_manager_init(uint32_t receive_timeout)\`

Initializes the event manager, setting up a queue for events and creating the event manager thread to handle event dispatching. You can specify a \`receive_timeout\` for how long the event manager waits for new events from the queue.

### \`event_manager_register_event(uint32_t event_id)\`

Registers a new event by adding it to the linked list of events.

### \`event_manager_subscribe_event(uint32_t event_id, task_context_t *task)\`

Subscribes a task to the specified event by adding the task to the event's subscriber list.

### \`event_manager_publish_event(events_t *event)\`

Publishes an event by sending it to the event manager queue. The event manager will dispatch the event to all subscribers.

### \`event_manager_unsubscribe_event(uint32_t event_id, task_context_t *task)\`

Unsubscribes a task from an event by removing the task from the event's subscriber list.

### \`event_manager_unregister_event(uint32_t event_id)\`

Unregisters an event, removing the event and all its subscribers from the system.

## Example Usage

Here’s an example of how to use the event manager:

\`\`\`c
event_manager_init(1000);  // Initialize event manager with a 1-second timeout

// Register an event
event_manager_register_event(1);

// Create a task and subscribe it to the event
task_context_t task1 = { /* Initialize task context */ };
event_manager_subscribe_event(1, &task1);

// Publish an event
events_t event = { .id = 1 };
event_manager_publish_event(&event);

// Unsubscribe the task from the event
event_manager_unsubscribe_event(1, &task1);

// Unregister the event
event_manager_unregister_event(1);
\`\`\`

## License

This project is licensed under the MIT License.
