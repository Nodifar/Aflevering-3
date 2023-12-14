#include "util.h"
#include <math.h>
#include <stdio.h>
#include <assert.h>

int smallest_idx(int k, int *closest) {
  for(int i = k-1; i>=0; i--) {
    if(closest[i] == -1) {
      continue;
    }
    return i;
  }
  return 0;
}

double distance(int d, const double *x, const double *y) {
  double sum = 0;
  for(int i = 0; i < d; i++) {
    sum += (x[i] - y[i]) * (x[i] - y[i]);
  }
  return sqrt(sum);
}

int insert_if_closer(int k, int d, const double *points, int *closest, const double *query, int candidate) {
  for(int i = 0; i < k; i++) {
    // if closest is -1, then simply add the point to closest
    if (closest[i] == -1) {
      closest[i] = candidate;
      return 1;
    }
    double dist1 = distance(d, &points[candidate*d], query);
    double dist2 = distance(d, &points[closest[i]*d], query);
    // Find spot where candidate fits, if there is any
    if (dist1 < dist2) {
      // move items down one to make space for candidate, this removes the last item(or a -1 if there is any)
      for(int j = k-1; j > i; j--) {
        closest[j] = closest[j-1];
      }
      closest[i] = candidate;
      return 1;
    }
  }
  return 0;
}
