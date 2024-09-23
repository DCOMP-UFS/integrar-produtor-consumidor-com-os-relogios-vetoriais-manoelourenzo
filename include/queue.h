#ifndef QUEUE
#define QUEUE

#include <pthread.h>
#include "clock.h"

#define BUFFER_SIZE 256

typedef struct queue_t {
  Clock queue[BUFFER_SIZE];
  int head;
  int tail;
  int size;
  int capacity;
} queue_t;

queue_t* init_queue();
int is_empty(queue_t* q);
int is_full(queue_t* q);
void enqueue(
  queue_t* q,
  Clock c,
  pthread_mutex_t* mutex,
  pthread_cond_t* queueNotFull,
  pthread_cond_t* queueNotEmpty
);
void dequeue(
  queue_t* q,
  Clock c,
  pthread_mutex_t* mutex,
  pthread_cond_t* queueNotFull,
  pthread_cond_t* queueNotEmpty
);
void* consumerBehavior(void *consumer_args);
void* producerBehavior(void *producer_args);

#endif // !QUEUE
