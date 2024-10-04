#include <stdlib.h>
#include "queue.h"

queue_t* init_queue() {
  queue_t* q = (queue_t*) malloc(sizeof(queue_t));
  q->size = 0;
  q->head = 0;
  q->tail = -1;
  q->capacity = BUFFER_SIZE;
  return q;
}

int is_empty(queue_t* q) {
  return q->size == 0;
}

int is_full(queue_t* q) {
  return q->size == q->capacity;
}

void enqueue(
  queue_t* q,

  data d,
  pthread_mutex_t* mutex,
  pthread_cond_t* queueNotFull,
  pthread_cond_t* queueNotEmpty)
  {

  
  while (is_full(q)) {
    pthread_cond_wait(queueNotFull, mutex);
  }

  q->tail = (q->tail + 1) % q->capacity;
  q->queue[q->tail] = d;
  q->size++;

  pthread_cond_signal(queueNotEmpty);
}

data dequeue(
  queue_t* q,
  pthread_mutex_t* mutex,
  pthread_cond_t* queueNotFull,
  pthread_cond_t* queueNotEmpty
) {
  while (is_empty(q)) {
    pthread_cond_wait(queueNotEmpty, mutex);
  }

  data d = q->queue[q->head];
  q->head = (q->head + 1) % q->capacity;
  q->size--;

  print_clock(d.c);

  pthread_cond_signal(queueNotFull);

  return d;
}

void free_queue(queue_t* q)
{
  free(q->queue);
  free(q);
}
