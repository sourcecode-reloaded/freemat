#include <stdio.h>
#include <ctype.h>
#include <string.h>

int addArrays(int N, float *a, float *b, float *c) {
  int i;
 
  for (i=0;i<N;i++)
   c[i] = a[i] + b[i];
  return N;
}

int sum_string(char* p) {
  printf("%s",p);
  return strlen(p);
}

int upper_string(char* p) {
  while (*p) {
    *p = toupper(*p);
    p++;
  }
  return strlen(p);
}

void square_it(double *t) {
  *t = (*t)*(*t);
}
