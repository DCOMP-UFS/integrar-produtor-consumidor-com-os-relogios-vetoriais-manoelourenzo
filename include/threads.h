#ifndef THREADS
#define THREADS

#include <clock.h>
#include <queue.h>

typedef struct thread_dependencies {
  pthread_mutex_t* mutex;
  pthread_cond_t* queueNotFull;
  pthread_cond_t* queueNotEmpty;
} thread_dependencies;

typedef struct args_t {
  queue_t* q;
  thread_dependencies t;
} args_t;

typedef struct main_args {
  int pid;
  queue_t* input;
  queue_t* output;
  Clock* c;
  thread_dependencies input_t;
  thread_dependencies output_t;
} main_args;

void *handle_receiver(void *arg);
void *handle_main_thread0(void *arg);
void *handle_main_thread1(void *arg);
void *handle_main_thread2(void *arg);
void *handle_sender(void *arg);

void Send(
    int pid_sender,
    int pid_receiver,
    queue_t* output,
    Clock* c,
    thread_dependencies t
);

void Receive(
    int pid_receiver,
    queue_t* input,
    Clock* c,
    thread_dependencies t
);

void Event(int pid, Clock* c);

#endif // !THREADS
