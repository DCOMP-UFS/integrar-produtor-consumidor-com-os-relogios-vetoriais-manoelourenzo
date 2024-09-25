#include <mpi.h>
#include <stdio.h>
#include <pthread.h>
#include "threads.h"
#include "clock.h"
#include "queue.h"

void *handle_receiver(void *arg) 
{
  args_t* args = (args_t*)arg;

  while (1) 
  {
    // mpi receive
    data d;
    MPI_Recv(
      &d,
      sizeof(d),
      MPI_BYTE,
      MPI_ANY_SOURCE,
      NULL,
      MPI_COMM_WORLD,
      MPI_STATUS_IGNORE
    );

    printf("%d received from %d\n", d.rank_receiver, d.rank_sender);
    // put the received clock in the queue
    enqueue(
      args->q,
      d,
      args->t.mutex,
      args->t.queueNotFull,
      args->t.queueNotEmpty
    );
  }
}

void *handle_sender(void *arg)
{
  args_t* args = (args_t*)arg;
  while (1) 
  {
    // consume clock to be sended from queue 
    data d = dequeue(
      args->q,
      args->t.mutex,
      args->t.queueNotEmpty,
      args->t.queueNotFull
    );

    // mpi send
    MPI_Send(
      &d,
      sizeof(data),
      MPI_BYTE,
      d.rank_receiver,
      0,
      MPI_COMM_WORLD
    );
  }
}

void *handle_main_thread0(void *arg) 
{
  main_args* args = (main_args*)arg;
  //exemplo da chamada da função event
  event(args->pid, args->c, args->t);
  send(args->pid, 1, args->output, args->c, args->t);
  receive(args->pid, args->input, args->c, args->t);
  send(args->pid, 2, args->output, args->c, args->t);
  receive(args->pid, args->input, args->c, args->t);
  send(args->pid, 1, args->output, args->c, args->t);
  event(args->pid, args->c, args->t);
  print_clock(*args->c);
}

void *handle_main_thread1(void *arg) 
{
  main_args* args = (main_args*)arg;
  send(args->pid, 0, args->output, args->c, args->t);
  receive(args->pid, args->input, args->c, args->t);
  receive(args->pid, args->input, args->c, args->t);
  print_clock(*args->c);
}

void *handle_main_thread2(void *arg) 
{
  main_args* args = (main_args*)arg;
  event(args->pid, args->c, args->t);
  send(args->pid, 0, args->output, args->c, args->t);
  receive(args->pid, args->input, args->c, args->t);
  print_clock(*args->c);
}

void receive(
  int pid_receiver,
  queue_t* input,
  Clock* c,
  thread_dependencies t
){  
  // atualiza o clock atual
  pthread_mutex_lock(t.mutex);
  c->c[pid_receiver]++;
  pthread_mutex_unlock(t.mutex);

  // receive data from the queue
  data d = dequeue(
    input,
    t.mutex,
    t.queueNotFull,
    t.queueNotEmpty
  );
  printf("%d received from %d\n", d.rank_receiver, d.rank_sender);

  // update the clock
  for (int i = 0; i < 3; i++)
  {
    if (d.c.c[i] > c->c[i])
    {
      c->c[i] = d.c.c[i];
    }
  }
}

void send(
  int pid_sender,
  int pid_receiver,
  queue_t* output,
  Clock* c,
  thread_dependencies t
){
  printf("%d sent to %d\n", pid_sender, pid_receiver);
  // atualiza o clock atual
  pthread_mutex_lock(t.mutex);
  c->c[pid_sender]++;
  pthread_mutex_unlock(t.mutex);

  // cria dado a ser inserido na fila intermediária
  data d = {
    .rank_sender = pid_sender,
    .rank_receiver = pid_receiver,
    .c = *c
  };

  // insere dado na fila intermediária
  enqueue(
    output,
    d,
    t.mutex,
    t.queueNotFull,
    t.queueNotEmpty
  );
}

void event(int pid, Clock* c, thread_dependencies t)
{
  printf("%d event\n", pid);
  // atualiza o clock atual
  pthread_mutex_lock(t.mutex);
  c->c[pid]++;
  pthread_mutex_unlock(t.mutex);
}

