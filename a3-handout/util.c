#include "util.h"
#include <math.h>
#include <stdio.h>
#include <assert.h>

int smallest_idx(int k, int *closest) {
  for(int i = k-1; i >= 0; i--) {
    if(closest[i] != -1) {
      return i;
    }
  }
  return -1;
}


double distance(int d, const double *x, const double *y) {
  double sum = 0;
  for(int i = 0; i < d; i++) {
    sum += (x[i] - y[i]) * (x[i] - y[i]);
  }
  return sqrt(sum);
}

int insert_if_closer(int k, int d, const double *points, int *closest, const double *query, int candidate) {
  double candidateDistance = distance(d, &points[candidate * d], query);

  for(int i = 0; i < k; i++) {
      if (closest[i] == -1) {
          closest[i] = candidate;
          return 1;
      }
      double existingDistance = distance(d, &points[closest[i] * d], query);
      
      if (candidateDistance < existingDistance || (candidateDistance == existingDistance && candidate < closest[i])) {
          for(int j = k-1; j > i; j--) {
              closest[j] = closest[j - 1];
          }
          closest[i] = candidate;
          return 1;
      }
  }
  return 0;
}

