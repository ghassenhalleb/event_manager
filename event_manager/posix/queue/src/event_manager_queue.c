#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <stdint.h>
#include <string.h>

#include "event_manager_queue.h"

static void ms_to_timespec(uint32_t timeout_ms, struct timespec* ts);


queue_t* queue_init(uint32_t element_size)
{
	queue_t* q = malloc(sizeof(queue_t));
    q->front = q->rear = NULL;
    if (pthread_mutex_init(&q->mutex, NULL) !=0)
    {
    	return NULL;
    }
    if (pthread_cond_init(&q->cond, NULL) != 0)
    {
    	return NULL;
    }
    q->size = 0;
    q->element_size = element_size;
    return q;
}

static void ms_to_timespec(uint32_t timeout_ms, struct timespec* ts) {
    clock_gettime(CLOCK_REALTIME, ts);
    ts->tv_sec += timeout_ms / 1000;
    ts->tv_nsec += (timeout_ms % 1000) * 1000000;
    if (ts->tv_nsec >= 1000000000) {
        ts->tv_sec += 1;
        ts->tv_nsec -= 1000000000;
    }
}

uint32_t queue_send(queue_t *q, void *data)
{
	queue_element_t *element = (queue_element_t *)malloc(sizeof(queue_element_t));

	if (element == NULL)
	{
		return -1;
	}
	element->data = malloc(q->element_size);
	memcpy(element->data, data, q->element_size);
	element->next = NULL;

	if (q->rear == NULL)
	{
		q->front = q->rear = element;
	}
	else
	{
	    q->rear->next = element;
	    q->rear = element;
	}
	q->size++;

	pthread_cond_signal(&q->cond);
	pthread_mutex_unlock(&q->mutex);

    return 0;
}


void* queue_receive(queue_t* q, uint32_t timeout_ms) {

	struct timespec timeout;

	ms_to_timespec(timeout_ms, &timeout);
    pthread_mutex_lock(&q->mutex);

    while (q->size == 0) {
        int ret;
        if (timeout_ms) {
            ret = pthread_cond_timedwait(&q->cond, &q->mutex, &timeout);
            if (ret == ETIMEDOUT) {
                pthread_mutex_unlock(&q->mutex);
                return NULL;  // Timeout occurred
            }
        } else {
            pthread_cond_wait(&q->cond, &q->mutex);
        }
    }

    queue_element_t* temp = q->front;
    void* data = temp->data;
    q->front = q->front->next;

    if (q->front == NULL) {
        q->rear = NULL;
    }

    q->size--;
    free(temp);

    pthread_mutex_unlock(&q->mutex);
    return data;
}

void denit_queue(queue_t* q) {
    pthread_mutex_lock(&q->mutex);

    queue_element_t* current = q->front;
    while (current != NULL) {
    	queue_element_t* temp = current;
        current = current->next;
        free(temp);
    }

    pthread_mutex_unlock(&q->mutex);
    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->cond);
}

