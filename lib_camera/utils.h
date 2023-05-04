#include <stdint.h>

#define PRINT_TIME(a,b) printf("%d\n", b - a);

int measure_time(){
  int y = 0;
  asm volatile("gettime %0": "=r"(y));
  return y;
}