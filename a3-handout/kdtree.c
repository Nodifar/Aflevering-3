#include "kdtree.h"
#include "sort.h"
#include "util.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

struct node {
  int point_index;
  int axis;
  struct node *left;
  struct node *right;
};

struct kdtree {
  int d;
  const double *points;
  struct node* root;
};

struct args_struct {
  int d;
  int axis;
  const double* points;
};

int compare (const int *p1, const int *p2, struct args_struct *args) {
  int d = args->d;
  const double* points = args->points;
  double ax1 = points[*p1*d + args->axis];
  double ax2 = points[*p2*d + args->axis];
  if(ax1 < ax2) {
    return -1;
  } else if (ax1 > ax2) {
    return 1;
  } else {
    return 0;
  }
}

struct node* kdtree_create_node(int d, const double *points, int depth, int n, int *indexes) {
  if (n == 0) {
    return NULL;
  }
  struct node* new_node = malloc(sizeof(struct node));
  new_node->axis = depth % d;
  struct args_struct arg;
  arg.d = d;
  arg.axis = new_node->axis;
  arg.points = points;
  hpps_quicksort(indexes, n, sizeof(int), (int(*)(const void *, const void *, void* ))compare, &arg);
  new_node->point_index = indexes[n/2];
  new_node->left = kdtree_create_node(d, points, depth+1, n/2, indexes);
  new_node->right = kdtree_create_node(d, points, depth+1, n-n/2-1, indexes+n/2+1);

  return new_node;
}

struct kdtree *kdtree_create(int d, int n, const double *points) {
  struct kdtree *tree = malloc(sizeof(struct kdtree));
  tree->d = d;
  tree->points = points;

  int *indexes = malloc(sizeof(int) * n);

  for (int i = 0; i < n; i++) {
    indexes[i] = i;
  }

  tree->root = kdtree_create_node(d, points, 0, n, indexes);

  free(indexes);

  return tree;
}

void kdtree_free_node(struct node *node) {
    if (node != NULL) {
        kdtree_free_node(node->left);
        kdtree_free_node(node->right);
        free(node);
    }
}


void kdtree_free(struct kdtree *tree) {
  kdtree_free_node(tree->root);
  free(tree);
}

void kdtree_knn_node(const struct kdtree *tree, int k, const double* query, int *closest, double *radius, const struct node *node) {
    if (node == NULL) {
        return;
    } 
    int d = tree->d;
    const double* points = tree->points;
    int idx = smallest_idx(k, closest);

    if(insert_if_closer(k, d, points, closest, query, node->point_index)) {
        idx = smallest_idx(k, closest); // Re-evaluate idx as it might have changed
        *radius = distance(d, &points[closest[idx]*d], query);
    }

    double diff = points[node->point_index*d+node->axis] - query[node->axis];
    if(d == 1 || diff >= 0 || *radius > fabs(diff)) {
      kdtree_knn_node(tree, k, query, closest, radius, node->left);
    }
    if(d == 1 || diff <= 0 || *radius > fabs(diff)) {
      kdtree_knn_node(tree, k, query, closest, radius, node->right);
    }
}


int* kdtree_knn(const struct kdtree *tree, int k, const double* query) {
  int* closest = malloc(k * sizeof(int));
  double radius = INFINITY;

  for (int i = 0; i < k; i++) {
    closest[i] = -1;
  }

  kdtree_knn_node(tree, k, query, closest, &radius, tree->root);
  
  return closest;
}

void kdtree_svg_node(double scale, FILE *f, const struct kdtree *tree,
                     double x1, double y1, double x2, double y2,
                     const struct node *node) {
  if (node == NULL) {
    return;
  }

  double coord = tree->points[node->point_index*2+node->axis];
  if (node->axis == 0) {
    // x split
    fprintf(f, "<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" stroke-width=\"1\" stroke=\"black\" />\n",
            coord*scale, y1*scale, coord*scale, y2*scale);
    kdtree_svg_node(scale, f, tree,
                    x1, y1, coord, y2,
                    node->left);
    kdtree_svg_node(scale, f, tree,
                    coord, y1, x2, y2,
                    node->right);
  } else {
    // y split
    fprintf(f, "<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" stroke-width=\"1\" stroke=\"black\" />\n",
            x1*scale, coord*scale, x2*scale, coord*scale);
    kdtree_svg_node(scale, f, tree,
                    x1, y1, x2, coord,
                    node->left);
    kdtree_svg_node(scale, f, tree,
                    x1, coord, x2, y2,
                    node->right);
  }
}

void kdtree_svg(double scale, FILE* f, const struct kdtree *tree) {
  assert(tree->d == 2);
  kdtree_svg_node(scale, f, tree, 0, 0, 1, 1, tree->root);
}
