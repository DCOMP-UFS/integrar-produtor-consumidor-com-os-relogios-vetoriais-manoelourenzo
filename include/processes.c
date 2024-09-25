#include <stdio.h>
#include <pthread.h>
#include "clock.h"
#include "queue.h"
#include "threads.h"
#include "processes.h"

void init_process(int rank, void* (*handle_func)(void*)) 
{
  pthread_mutex_t mutex;
  pthread_cond_t queueNotFull;
  pthread_cond_t queueNotEmpty;

  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&queueNotFull, NULL);
  pthread_cond_init(&queueNotEmpty, NULL);

  queue_t* inputQueue = init_queue();
  queue_t* outputQueue = init_queue();

  Clock clock = {
    .c = {0}
  };

  main_args main = {
    .pid = rank,
    .c = &clock,
    .t = {
      .mutex = &mutex,
      .queueNotFull = &queueNotFull,
      .queueNotEmpty = &queueNotEmpty
    },
    .input = inputQueue,
    .output = outputQueue
  };

  args_t receiver = {
    .q = inputQueue,
    .t = {
      .mutex = &mutex,
      .queueNotFull = &queueNotFull,
      .queueNotEmpty = &queueNotEmpty
    }
  };

  args_t sender = {
    .q = outputQueue,
    .t = {
      .mutex = &mutex,
      .queueNotFull = &queueNotFull,
      .queueNotEmpty = &queueNotEmpty,
      .queueNotFull = &queueNotFull
    }
  };

  pthread_t threads[3];

  pthread_create(&threads[0], NULL, &handle_receiver, (void*)&receiver);
  pthread_create(&threads[1], NULL, handle_func, (void*)&main);
  pthread_create(&threads[2], NULL, &handle_sender, (void*)&sender);

  for (int i = 0; i < PROC_NUM; i++) {
    pthread_join(threads[i], NULL);
  }

  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&queueNotEmpty);
  pthread_cond_destroy(&queueNotFull);

  free_queue(inputQueue);
  free_queue(outputQueue);
  return;
}

void process0(int rank) 
{
  printf("initializing process 0\n");
  init_process(rank, &handle_main_thread0);
}

void process1(int rank) 
{
  printf("initializing process 1\n");
  init_process(rank, &handle_main_thread1);
}

void process2(int rank) 
{
  printf("initializing process 2\n");
  init_process(rank, &handle_main_thread2);
}
