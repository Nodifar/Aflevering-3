#include "io.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char **argv) {
    FILE *points_f = fopen(argv[1], "r");
    FILE *queries_f = fopen(argv[2], "r");
    FILE *indexes_f = fopen(argv[3], "r");
    assert(points_f && queries_f && indexes_f);

    int n_points, d_points, n_queries, d_queries, n_indexes, k;
    double *points = read_points(points_f, &n_points, &d_points);
    double *queries = read_points(queries_f, &n_queries, &d_queries);
    int *indexes = read_indexes(indexes_f, &n_indexes, &k);
    assert(points && queries && indexes);

    fclose(points_f);
    fclose(queries_f);
    fclose(indexes_f);

    int test_passed = 1;
    for (int i = 0; i < n_queries; i++) {
        for (int j = 0; j < n_points; j++) {
            double dist = distance(d_points, &queries[i * d_queries], &points[j * d_points]);
            int is_neighbor = 0;
            double max_neighbor_dist = -1.0;
            for (int l = 0; l < k; l++) {
                int neighbor_idx = indexes[i * k + l];
                double neighbor_dist = distance(d_points, &queries[i * d_queries], &points[neighbor_idx * d_points]);
                if (j == neighbor_idx) is_neighbor = 1;
                if (neighbor_dist > max_neighbor_dist) max_neighbor_dist = neighbor_dist;
            }
            if (!is_neighbor && dist < max_neighbor_dist) {
                printf("Test failed for query %d: Point %d is closer than a neighbor.\n", i, j);
                test_passed = 0;
                break;
            }
        }
        if (!test_passed) break;
    }
    if (test_passed) {
        printf("Test passed: All query points have correct nearest neighbors.\n");
        printf("\n");
    }
    free(points);
    free(queries);
    free(indexes);
    return 0;
}
