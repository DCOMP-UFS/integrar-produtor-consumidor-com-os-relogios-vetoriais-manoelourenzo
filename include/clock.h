#ifndef CLOCK
#define CLOCK

#define THREAD_NUM 3

typedef struct Clock {
  int c[THREAD_NUM];
} Clock;

void print_clock(Clock c);

#endif // !CLOCK
