#include <stdio.h>
#include <pthread.h>
#include <clock.h>
#include <queue.h>
#include <threads.h>
#include <processes.h>

void init_process(int rank, void* (*handle_func)(void*)) 
{
  pthread_mutex_t input_mutex;
  pthread_cond_t input_queueNotFull;
  pthread_cond_t input_queueNotEmpty;

  pthread_mutex_init(&input_mutex, NULL);
  pthread_cond_init(&input_queueNotFull, NULL);
  pthread_cond_init(&input_queueNotEmpty, NULL);

  pthread_mutex_t output_mutex;
  pthread_cond_t output_queueNotFull;
  pthread_cond_t output_queueNotEmpty;

  pthread_mutex_init(&output_mutex, NULL);
  pthread_cond_init(&output_queueNotFull, NULL);
  pthread_cond_init(&output_queueNotEmpty, NULL);

  queue_t* inputQueue = init_queue();
  queue_t* outputQueue = init_queue();

  Clock clock = {
    .c = {0}
  };

  main_args main = {
    .pid = rank,
    .c = &clock,
    .input_t = {
      .mutex = &input_mutex,
      .queueNotFull = &input_queueNotFull,
      .queueNotEmpty = &input_queueNotEmpty
    },
    .output_t = {
      .mutex = &output_mutex,
      .queueNotFull = &output_queueNotFull,
      .queueNotEmpty = &output_queueNotEmpty
    },
    .input = inputQueue,
    .output = outputQueue
  };

  args_t receiver = {
    .q = inputQueue,
    .t = {
      .mutex = &input_mutex,
      .queueNotFull = &input_queueNotFull,
      .queueNotEmpty = &input_queueNotEmpty
    }
  };

  args_t sender = {
    .q = outputQueue,
    .t = {
      .mutex = &output_mutex,
      .queueNotFull = &output_queueNotFull,
      .queueNotEmpty = &output_queueNotEmpty
    }
  };

  pthread_t threads[3];

  pthread_create(&threads[0], NULL, &handle_receiver, (void*)&receiver);
  pthread_create(&threads[1], NULL, handle_func, (void*)&main);
  pthread_create(&threads[2], NULL, &handle_sender, (void*)&sender);

  for (int i = 0; i < PROC_NUM; i++) {
    pthread_join(threads[i], NULL);
  }

  pthread_mutex_destroy(&input_mutex);
  pthread_cond_destroy(&input_queueNotEmpty);
  pthread_cond_destroy(&input_queueNotFull);

  pthread_mutex_destroy(&output_mutex);
  pthread_cond_destroy(&output_queueNotEmpty);
  pthread_cond_destroy(&output_queueNotFull);

  free_queue(inputQueue);
  free_queue(outputQueue);
  return;
}

void process0(int rank) 
{
  printf("process %d: starting\n", rank);
  init_process(rank, &handle_main_thread0);
}

void process1(int rank) 
{
  printf("process %d: starting\n", rank);
  init_process(rank, &handle_main_thread1);
}

void process2(int rank) 
{
  printf("process %d: starting\n", rank);
  init_process(rank, &handle_main_thread2);
}
