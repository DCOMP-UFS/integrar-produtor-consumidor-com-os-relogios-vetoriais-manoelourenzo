#ifndef THREADS
#define THREADS

#include "clock.h"
#include "queue.h"

typedef struct thread_dependencies {
  pthread_mutex_t* mutex;
  pthread_cond_t* queueNotFull;
  pthread_cond_t* queueNotEmpty;
} thread_dependencies;

typedef struct sender_args {
  queue_t* q;
  thread_dependencies t;
} sender_args;

typedef struct main_args {
  int pid;
  queue_t* input;
  queue_t* output;
  Clock* c;
  thread_dependencies t;
} main_args;

typedef struct receiver_args {
  queue_t* q;
  thread_dependencies t;
} receiver_args;

void *handle_receiver(void *arg);
void *handle_main_thread(void *arg);
void *handle_sender(void *arg);

void send(int pid_sender, int pid_receiver, queue_t* output, Clock* c, thread_dependencies t);
void receive(int pid_sender, int pid_receiver, queue_t* input, Clock* c, thread_dependencies t);
void event(int pid, Clock* c, thread_dependencies t);

#endif // !THREADS
