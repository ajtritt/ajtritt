#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sample.h"


typedef struct tdist {
    int* id;
    double* dist;
    int len;
} tdist;

tdist* leaf_distance(NODE * a, double * cum_dist) {

    tdist* ret = (tdist*) malloc(sizeof(tdist));
    if (a->node_id > -1){
        ret->id = (int*) malloc(sizeof(int));
        ret->dist = (double*) malloc(sizeof(int));
        ret->id[0] = a->node_id;
        ret->dist[0] = a->blen;
        ret->len = 1;
    } else {
        tdist* l_dist;
        tdist* r_dist;
        //#pragma omp task shared(l_dist)
        l_dist = leaf_distance(a->left, cum_dist);
        //#pragma omp task shared(r_dist)
        r_dist = leaf_distance(a->right, cum_dist);
        //#pragma omp taskwait
        ret->len = l_dist->len + r_dist->len;
        ret->id = (int*) malloc(sizeof(int)*ret->len);
        ret->dist = (double*) malloc(sizeof(double)*ret->len);
        int i;
        for (i = 0; i < l_dist->len; i++) { // copy left leaf distances
            ret->id[i] = l_dist->id[i];
            ret->dist[i] = l_dist->dist[i] + a->blen;
        }
        for (i = 0; i < r_dist->len; i++) { // copy right leaf distances
            ret->id[i+l_dist->len] = r_dist->id[i];
            ret->dist[i+l_dist->len] = r_dist->dist[i] + a->blen;
        }
        // tally distances
        int j;
        double dist_ij;
        for (i = 0; i < l_dist->len; i++) {
            for (j = 0; j < r_dist->len; j++) {
                dist_ij = l_dist->dist[i] + r_dist->dist[j];
                cum_dist[l_dist->id[i]] += dist_ij;
                cum_dist[r_dist->id[j]] += dist_ij;
            }
        }
        free(l_dist);
        free(r_dist);
    }
    return ret;
}

int main() {
    // allocate array for cumulative distances
    int n_nodes = 3;
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
