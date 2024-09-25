#ifndef PROCESSES
#define PROCESSES

void init_process(int pid, void* (*handle_func)(void*));

void process0(int rank);
void process1(int rank);
void process2(int rank);

#endif // !PROCESSES
