#ifndef CLOCK
#define CLOCK

#define PROC_NUM 3

typedef struct Clock {
  int c[PROC_NUM];
} Clock;

void print_clock(Clock c);

#endif // !CLOCK
