#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

typedef struct _element {
    void* data;
    struct timespec timestamp;
    struct _element * next;
} queue_element_t;

typedef struct {
    queue_element_t* front;
    queue_element_t* rear;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    uint32_t size;
    uint32_t element_size;
} queue_t;

queue_t*  queue_init(uint32_t element_size);
void* queue_receive(queue_t* q, uint32_t timeout_ms);
uint32_t queue_send(queue_t *q, void *data);

#endif /* _QUEUE_H_ */
