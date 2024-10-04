#include <mpi.h>
#include <stdio.h>
#include <pthread.h>
#include <threads.h>
#include <clock.h>
#include <queue.h>

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
      0,
      MPI_COMM_WORLD,
      MPI_STATUS_IGNORE
    );

    // put the received clock in the queue
    enqueue(
      args->q,
      d,
      args->t.mutex,
      args->t.queueNotFull,
      args->t.queueNotEmpty
    );
  }
  return NULL;
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
  return NULL;
}

void *handle_main_thread0(void *arg) 
{
  main_args* args = (main_args*)arg;
  //exemplo da chamada da função event
  Event(args->pid, args->c);
  Send(args->pid, 1, args->output, args->c, args->output_t);
  Receive(args->pid, args->input, args->c, args->input_t);
  Send(args->pid, 2, args->output, args->c, args->output_t);
  Receive(args->pid, args->input, args->c, args->input_t);
  Send(args->pid, 1, args->output, args->c, args->output_t);
  Event(args->pid, args->c);
  printf("process %d: final ", args->pid);
  print_clock(*args->c);
  return NULL;
}

void *handle_main_thread1(void *arg) 
{
  main_args* args = (main_args*)arg;
  Send(args->pid, 0, args->output, args->c, args->output_t);
  Receive(args->pid, args->input, args->c, args->input_t);
  Receive(args->pid, args->input, args->c, args->input_t);
  printf("process %d: final ", args->pid);
  print_clock(*args->c);
  return NULL;
}

void *handle_main_thread2(void *arg) 
{
  main_args* args = (main_args*)arg;
  Event(args->pid, args->c);
  Send(args->pid, 0, args->output, args->c, args->output_t);
  Receive(args->pid, args->input, args->c, args->input_t);
  printf("process %d: final ", args->pid);
  print_clock(*args->c);
  return NULL;
}

void Receive(
  int pid_receiver,
  queue_t* input,
  Clock* c,
  thread_dependencies t
){  

  // receive data from the queue
  data d = dequeue(
    input,
    t.mutex,
    t.queueNotFull,
    t.queueNotEmpty
  );

  // atualiza o clock atual
  c->c[pid_receiver]++;

  pthread_mutex_lock(t.mutex);
  // update the clock
  for (int i = 0; i < 3; i++)
  {
    if (d.c.c[i] > c->c[i])
    {
      c->c[i] = d.c.c[i];
    }
  }
  pthread_mutex_unlock(t.mutex);

  printf("process %d: received from %d and has the updated ", d.rank_receiver, d.rank_sender);
  print_clock(*c);
}

void Send(
  int pid_sender,
  int pid_receiver,
  queue_t* output,
  Clock* c,
  thread_dependencies t
){
  // atualiza o clock atual
  c->c[pid_sender]++;

  printf("process %d: sent to %d with ", pid_sender, pid_receiver);
  print_clock(*c);

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

void Event(int pid, Clock* c)
{
  // atualiza o clock atual
  c->c[pid]++;

  printf("process %d: registered an event and has ", pid);
  print_clock(*c);
}

