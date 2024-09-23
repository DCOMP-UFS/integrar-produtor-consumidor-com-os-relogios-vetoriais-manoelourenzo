#include <stdio.h>
#include "clock.h"

void print_clock(Clock c) {
  printf("clock: ( %i", c.c[0]);
  for (int i = 1; i < THREAD_NUM; i++) {
    printf(", %i", c.c[i]);
  }
  printf(")\n");
}
