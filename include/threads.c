#include "threads.h"
#include "queue.h"
#include <pthread.h>

void *handle_receiver(void *arg) 
{
  receiver_args args = *((receiver_args*) arg);

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

    // put the received clock in the queue
    enqueue(
      args.q,
      d,
      args.t.mutex,
      args.t.queueNotFull,
      args.t.queueNotEmpty
    );
  }
}

void *handle_sender(void *arg)
{
  sender_args args = *((sender_args*)arg);
  while (1) 
  {
    // consume clock to be sended from queue 
    data d = dequeue(
      args.q,
      args.t.mutex,
      args.t.queueNotEmpty,
      args.t.queueNotFull
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

void *handle_main_thread(void *arg) 
{
  main_args args = *((main_args*)arg);

  //exemplo de chamada da função receive
  receive(0, args.pid, args.input, args.c, args.t);

  //exemplo de chamada da função send
  send(args.pid, 0, args.output, args.c, args.t);

  //exemplo da chamada da função event
  event(args.pid, args.c, args.t);
}


void receive(int pid_sender, int pid_receiver, queue_t* input, Clock* c, thread_dependencies t)
{  
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

  // update the clock
  for (int i = 0; i < 3; i++)
  {
    if (d.c.c[i] > c->c[i])
    {
      c->c[i] = d.c.c[i];
    }
  }
}

void send(int pid_sender, int pid_receiver, queue_t* output, Clock* c, thread_dependencies t)
{
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
  // atualiza o clock atual
  pthread_mutex_lock(t.mutex);
  c->c[pid]++;
  pthread_mutex_unlock(t.mutex);
}
