#include "bruteforce.h"
#include "util.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

int* knn(int k, int d, int n, const double *points, const double* query) {
  int *closest = malloc(sizeof(int) * k);
  for(int i = 0; i < k; i++) {
    closest[i] = -1;
  }
  for(int i = 0; i < n; i++) {
    insert_if_closer(k, d, points, closest, query, i);
  }
  return closest;
}
