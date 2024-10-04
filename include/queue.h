#ifndef QUEUE
#define QUEUE

#include <pthread.h>
#include <clock.h>

#define BUFFER_SIZE 256

typedef struct data {
  int rank_sender;
  int rank_receiver;
  Clock c;
} data;

typedef struct queue_t {
  data queue[BUFFER_SIZE];
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
  data d,
  pthread_mutex_t* mutex,
  pthread_cond_t* queueNotFull,
  pthread_cond_t* queueNotEmpty
);

data dequeue(
  queue_t* q,
  pthread_mutex_t* mutex,
  pthread_cond_t* queueNotFull,
  pthread_cond_t* queueNotEmpty
);

void free_queue(queue_t* q);

#endif // !QUEUE
