#ifndef _EVENT_MANAGER_H
#define _EVENT_MANAGER_H

#include "event_manager.h"
#include "event_manager_tasks.h"
#include "event_manager_events.h"

int event_manager_init(uint32_t receive_timeout);
int event_manager_register_event(uint32_t event_id);
int event_manager_subscribe_event(uint32_t event_id, task_context_t *task);
int event_manager_unsubscribe_event(uint32_t event_id, task_context_t *task);
int event_manager_publish_event(events_t *event);
events_t * event_manager_wait_for_events(task_context_t *task, uint32_t timeout);

#endif //_EVENT_MANAGER_H