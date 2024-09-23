#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <mpi.h>
#include <pthread.h>

define THREAD_NUM 3
#define BUFFER_SIZE 256 // Númermo máximo de tarefas enfileiradas

typedef struct Clock {
  int c[THREAD_NUM];
} Clock;

typedef struct queue_t {
  Clock queue[BUFFER_SIZE];
  int head;
  int tail;
  int size;
  int capacity;
} queue_t;

typedef struct Task {
  int id;
  queue_t *q;
  Clock c;
} Task;

int main(int argc, char* argv[]) 
{
  queue_t *q = init_queue();

  pthread_mutex_init(&mutex, NULL);

  pthread_cond_init(&queueNotFull, NULL);
  pthread_cond_init(&queueNotEmpty, NULL);

  pthread_t consumers[THREAD_NUM];
  pthread_t producers[THREAD_NUM];

  Task *consumers_tasks = (Task*) calloc(THREAD_NUM, sizeof(Task));
  Task *producers_tasks = (Task*) calloc(THREAD_NUM, sizeof(Task));


  for (int i = 0; i < THREAD_NUM; i++)
  {  
    consumers_tasks[i] = (Task) {i, q, {0}};
    if (pthread_create(&consumers[i], NULL, &consumerBehavior, (void*) &consumers_tasks[i]) != 0) 
    {
      perror("Failed to create the thread");
    }  
  }
  for (int i = 0; i < THREAD_NUM; i++)
  {  
    producers_tasks[i] = (Task) {i, q, {0}};
    if (pthread_create(&producers[i], NULL, &producerBehavior, (void*) &producers_tasks[i]) != 0)
    {
      perror("Failed to create the thread");
    }  
  }

  srand(time(NULL));
  for (int i = 0; i < 500; i++);

  for (int i = 0; i < THREAD_NUM; i++)
  {  
    if (pthread_join(consumers[i], NULL) != 0)
    {
      perror("Failed to join the thread");
    }  
  }
  free(consumers_tasks);
  for (int i = 0; i < THREAD_NUM; i++)
  {  
    if (pthread_join(producers[i], NULL) != 0) 
    {
      perror("Failed to join the thread");
    }  
  }
  free(producers_tasks);

  free(q);
  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&queueNotEmpty);
  pthread_cond_destroy(&queueNotFull);
  return 0;
}

queue_t* init_queue() {
  queue_t* q = (queue_t*) malloc(sizeof(queue_t));
  q->size = 0;
  q->head = 0;
  q->tail = -1;
  q-> capacity = BUFFER_SIZE;
  return q;
}

int is_empty(queue_t* q) {
  return q->size == 0;
}

int is_full(queue_t* q) {
  return q->size == q->capacity;
}

void enqueue(queue_t* q, Clock c) {
  pthread_mutex_lock(&mutex);

  while (is_full(q)) {
    pthread_cond_wait(&queueNotFull, &mutex);
  }

  q->tail = (q->tail + 1) % q->capacity;
  q->queue[q->tail] = c;
  q->size++;

  pthread_mutex_unlock(&mutex);
  pthread_cond_signal(&queueNotEmpty);
}

void dequeue(queue_t* q, Clock c) {
  pthread_mutex_lock(&mutex);

  while (is_empty(q)) {
    pthread_cond_wait(&queueNotEmpty, &mutex);
  }

  c = q->queue[q->head];
  q->head = (q->head + 1) % q->capacity;
  q->size--;

  print_clock(c);

  pthread_mutex_unlock(&mutex);
  pthread_cond_signal(&queueNotFull);
}

void* consumerBehavior(void *consumer_args) {
  Task *t = (Task*) consumer_args;

  while (1) {
    dequeue(t->q, t->c);
    sleep(rand() % 5);
  }
  return NULL;
}

void* producerBehavior(void *producer_args) {
  Task *t = (Task*) producer_args;
  
  while (1) {
    t->c.c[t->id]++;
    enqueue(t->q, t->c);
    sleep(rand() % 5);
  }

  return NULL;
}

void print_clock(Clock c) {
  printf("clock: ( %i", c.c[0]);
  for (int i = 1; i < THREAD_NUM; i++) {
    printf(", %i", c.c[i]);
  }
  printf(")\n");
}

void Event(int pid, Clock *clock){
   clock->p[pid]++;   
}

void Send(int pid, Task *t) {
    pthread_mutex_lock(&mutex);  // Bloqueia a seção crítica.

    // Atualiza o relógio do processo de origem.
    t->c.c[pid]++;
    

    // Coloca a Task na fila intermediária (enqueue).
    enqueue(t->q, t->c);

    // Pega a Task mais antiga da fila para enviar.
    Clock task_to_send = t->q->queue[t->q->head];

    pthread_mutex_unlock(&mutex);  // Libera a seção crítica.

    // Envia a Task, utilizando o relógio atualizado da fila.
    MPI_Send(&task_to_send, sizeof(Clock), MPI_BYTE, t->id, 0, MPI_COMM_WORLD);

    printf("Process %d sent a message to Process %d, clock: (%d, %d, %d)\n",
           pid, t->id, task_to_send.c[0], task_to_send.c[1], task_to_send.c[2]);
}


void Receive(int pid, int pid_sender, Clock *clock){ // pid é o processo de destino, clock é o relógio do processo de origem
   clock->p[pid]++;   
   Clock received_clock;
   MPI_Recv(&received_clock, 3, MPI_INT, pid_sender, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

   for (int i = 0; i < 3; i++)
   {
      if (received_clock.p[i] > clock->p[i])
      {
         clock->p[i] = received_clock.p[i];
      }
   }
}

