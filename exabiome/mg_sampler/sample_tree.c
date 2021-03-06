#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
//#include <string.h>

void read_tree(char ** nwk_ptr, double * curr_dist, double * weights, char ** names, int * id) {
    char * nwk = *nwk_ptr;
    char *name = malloc(256), *blen = malloc(256);
    size_t name_n = 0, blen_n = 0;

    int nleaves = 0;

    int read_blen = 0;
    char c = *nwk;
    int s = *id;
    int m = *id;
    while (c != '\0') {
        if (c == '(') { // read left node
            nwk++;
            read_tree(&nwk, curr_dist, weights, names, id);
            m = *id;
        } else if (c == ')') { // done reading branch length
            blen[blen_n] = '\0';
            read_blen = 0;
            nwk++;
            break;
       } else if (c == ','){
            if (read_blen){ // add branch length to ret
                blen[blen_n] = '\0';
                break;
            } else {       // read right node
                nwk++;
                read_tree(&nwk, curr_dist, weights, names, id);
            }
        } else if (c == ':') {  // read branch length
            if (name_n > 0){   // we were reading a leaf node
                name[name_n] = '\0';
                nleaves = 1;
                names[(*id)++] = name;
            }
            read_blen = 1;
            nwk++;
        } else {           // reading branch lenght or name
            if (read_blen) {
                blen[blen_n++] = c;
            } else {
                name[name_n++] = c;
            }
            nwk++;
        }
        c = *nwk;
    }
    int e = *id;
    assert( m <  e);
    (*nwk_ptr) = nwk;
    int i, j;
    double tmp;
    for (i = s; i < m; i++){
        for (j = m; j < e; j++){
            tmp = curr_dist[i] + curr_dist[j];
            weights[i] += tmp;
            weights[j] += tmp;
        }
    }
    tmp = atof(blen);
    for (i = s; i < m; i++) {
        curr_dist[i] += tmp;
    }
    for (i = m; i < e; i++) {
        curr_dist[i] += tmp;
    }
    free(blen);
}

int binsearch(double * weights, int l, int r, double val){
    if (l == r)
        return l;
    int mid = l + (r-l)/2;
    if (val < weights[mid]){
        return binsearch(weights, l, mid, val);
    } else {
        return binsearch(weights, mid+1, r, val);
    }
}

void shift_weights(double * cum, int idx, int len) {
    int i;
    double w = -cum[idx];
    if (idx > 0)
        w += cum[idx-1]; // weight of idx
    if (len > 1)
        w += cum[len-1] - cum[len-2]; // weight of last element
    for (i = idx; i < len-1; i++)
        cum[i] = cum[i] + w;
    cum[len-1] = -1.0;
}

int main(int argc, char ** argv) {
    if (argc != 3) {
        printf("Usage: sample_tree in.nwk n_samples\n");
        return -1;
    }

    FILE *file = fopen(argv[1], "r");
    char *code;
    size_t n = 0;
    int c;

    printf("Reading tree\n");
    if (file == NULL) return 1; //could not open file
    // compute file size
    fseek(file, 0, SEEK_END);
    long f_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    code = malloc(f_size);

    int nsamples = atoi(argv[2]);

    // read in newick string
    int internodes_n = 0;
    while ((c = fgetc(file)) != EOF) {
        if (c == 10 || c == 32 || c == 9) {
            continue;
        }
        if (c == 44)
            internodes_n++;
        code[n++] = (char)c;
    }
    code[n] = '\0';

    // allocate distance and weights
    int nleaves = internodes_n+1;
    if (nsamples > nleaves){
        printf("nsamples must be fewer than the number of leaves (%d)\n", nleaves);
        return 1;
    }
    double * curr_dist = (double *) malloc(sizeof(double)*(nleaves));
    double * weights = (double *) malloc(sizeof(double)*(nleaves));
    char ** names = (char **) malloc(sizeof(char*)*(nleaves));
    int i;
    for (i = 0; i <= internodes_n; i++){
        curr_dist[i] = 0.0;
        weights[i] = 0.0;
    }

    // parse newick string and compute weights
    i = 0;
    int * id = &i;
    printf("Computing distances on tree (%d leaves)\n", nleaves);
    read_tree(&code, curr_dist, weights, names, id);
    printf("#found the following leaves\n");
    for (i = 0; i < nleaves; i++) {
        printf("#%s\n", names[i]);
    }

    // compute cumulative weights
    double * cum_weights = (double*) malloc(sizeof(double)*nleaves);
    double sum = 0.0;
    for (i = 0; i < nleaves; i++){
        sum += weights[i];
        cum_weights[i] = sum;
    }

    // allocate node ids to keep track of samples
    int * node_ids = (int*) malloc(nleaves*sizeof(int));
    for (i = 0; i < nleaves; i++){
        node_ids[i] = i;
    }

    // sample leaves
    int idx;
    int rem_leaves = nleaves;
    double U;
    int tmp;
    printf("Randomly sampling %d leaves\n", nsamples);
    for (i = 0; i < nsamples; i++){
        U = cum_weights[rem_leaves-1]*rand()/RAND_MAX;  // draw random number
        idx = binsearch(cum_weights, 0, rem_leaves, U); // get index
        shift_weights(cum_weights, idx, rem_leaves);    // update weights
        tmp = node_ids[idx];
        node_ids[idx] = node_ids[rem_leaves-1];
        node_ids[rem_leaves] = tmp;
        rem_leaves--;
    }

    // compute read abundance
    double * abundance = (double *) malloc(sizeof(double)*nsamples);
    sum = 0.0;
    for (i = 0; i < nsamples; i++){
        abundance[i] = -log(-((double) rand())/RAND_MAX + 1);
        sum += abundance[i];
    }
    for (i = 0; i < nsamples; i++)
        abundance[i] = abundance[i]/sum;

    printf("Printing samples\n");
    printf("img_taxon_oid\tabundance\n");
    for (i = rem_leaves; i < nleaves; i++){
        printf("%s\t%0.12f\n", names[node_ids[i]], abundance[i-rem_leaves]);
    }
}
