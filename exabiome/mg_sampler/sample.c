#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sample.h"


int main() {
    // allocate array for cumulative distances
    int n_nodes = 2;
    double * cum_dist = (double *) malloc(sizeof(double)*n_nodes);
    int i;
    for (i = 0; i < n_nodes; i++){
        cum_dist[i] = 0;
    }
    NODE A, B, C, AB, ROOT;
    A = (NODE) {.node_id = 0, .blen = 0.2};
    B = (NODE) {.node_id = 1, .blen = 0.3};
    C = (NODE) {.node_id = 2, .blen = 1.0};
    AB = (NODE) {.node_id = -1, .blen = 0.5, .left = &A, .right = &B};
    ROOT = (NODE) {.node_id = -2, .blen = -0.1, .left=&C, .right=&AB};

    leaf_distance(&ROOT, cum_dist);
    printf("Done computing total distances\n");
    printf("A = %.3f, should be 2.200\n", cum_dist[0]);   // A = 2.200
    printf("B = %.3f, should be 2.300\n", cum_dist[1]);   // B = 2.300
    printf("C = %.3f, should be 3.500\n", cum_dist[2]);   // C = 3.500

//    nleaves = 0;
//    char * tree = (char*) malloc(26*sizeof(char));
//    strcpy(tree, "(C:1.0,(B:0.3,A:0.2):0.5)\0");
//    int idx = 0;
//    int * idx_ptr = &idx;
//    NODE * READ_ROOT = read_node(tree, idx_ptr);
//
//    printf("Compute total distances from read tree\n");
//    leaf_distance(READ_ROOT);
//    printf("Done computing total distances\n");
//    printf("A = %.3f, should be 2.200\n", cum_dist[0]);   // A = 2.200
//    printf("B = %.3f, should be 2.300\n", cum_dist[1]);   // B = 2.300
//    printf("C = %.3f, should be 3.500\n", cum_dist[2]);   // C = 3.500
}
