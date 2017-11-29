#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define INTERNAL_NODE -1

struct NODE {
    struct NODE * left;
    struct NODE * right;
    int node_id;
    double blen;
    char* name;
};

struct TREE {
    struct NODE * root;
    int nleaves;
    char ** names;
};

struct TREE * read_node_rec(char ** nwk_ptr, int * id) {
    char * nwk = *nwk_ptr;
    char *name = malloc(256), *blen = malloc(256);
    size_t name_n = 0, blen_n = 0;

    struct NODE * root = (struct NODE *) malloc(sizeof(struct NODE));
    root->left = NULL;
    root->right = NULL;
    root->node_id = -1;
    int nleaves = 0;

    int read_blen = 0;
    char c = *nwk;
    while (c != '\0') {
        if (c == '(') { // read left node
            nwk++;
            struct TREE * subtree = read_node_rec(&nwk, id);
            root->left = subtree->root;
            nleaves += subtree->nleaves;
        } else if (c == ')') { // done reading branch length
            blen[blen_n] = '\0';
            root->blen = atof(blen);
            read_blen = 0;
            nwk++;
            break;
       } else if (c == ','){
            if (read_blen){ // add branch length to ret
                blen[blen_n] = '\0';
                root->blen = atof(blen);
                break;
            } else {       // read right node
                nwk++;
                struct TREE * subtree = read_node_rec(&nwk, id);
                root->right = subtree->root;
                nleaves += subtree->nleaves;
            }
        } else if (c == ':') {  // read branch length
            if (name_n > 0){   // we were reading a leaf node
                name[name_n] = '\0';
                nleaves = 1;
                root->name = name;
                root->node_id = (*id)++;
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
    (*nwk_ptr) = nwk;
    struct TREE * ret = (struct TREE *) malloc(sizeof(struct TREE));
    ret->root = root;
    ret->nleaves = nleaves;
    free(blen);
    return ret;
}

struct TREE * read_node(char * nwk) {
    int id = 0;
    struct TREE * ret = read_node_rec(&nwk, &id);
    return ret;
}

void get_names(struct NODE * node, char ** names){
    if (!node->left){
        names[node->node_id] = node->name;
    } else {
        node->node_id = -1;
        get_names(node->left, names);
        get_names(node->right, names);
    }
}

struct TREE * read_tree(char * nwk){
    struct TREE * ret = read_node(nwk);
    ret->names = (char**)malloc(ret->nleaves*sizeof(char*));
    get_names(ret->root, ret->names);
    return ret;
}

void print_leaves(struct NODE * node){
    if (!node->left){
        printf("%s:%.3f id = %d\n", node->name, node->blen, node->node_id);
    } else {
        printf("internode: blen = %0.3f\n", node->blen);
        print_leaves(node->left);
        print_leaves(node->right);
    }
}

typedef struct tdist {
    int* id;
    double* dist;
    int len;
} tdist;

tdist* leaf_distance(struct NODE * a, double * cum_dist) {

    tdist* ret = (tdist*) malloc(sizeof(tdist));
    if (a->node_id == INTERNAL_NODE){
        tdist* l_dist;
        tdist* r_dist;
        l_dist = leaf_distance(a->left, cum_dist);
        r_dist = leaf_distance(a->right, cum_dist);
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
    } else {
        ret->id = (int*) malloc(sizeof(int));
        ret->dist = (double*) malloc(sizeof(int));
        ret->id[0] = a->node_id;
        ret->dist[0] = a->blen;
        ret->len = 1;
    }
    return ret;
}

double * compute_total_distances(struct TREE * tree) {
    double * ret = (double *) malloc((tree->nleaves)*sizeof(double));
    int i;
    for (i = 0; i < tree->nleaves; i++){
        ret[i] = 0.0;
    }
    leaf_distance(tree->root, ret);
    return ret;
}


struct TREE * read_nwk_tree(char *fileName) {
    FILE *file = fopen(fileName, "r");
    char *code;
    size_t n = 0;
    int c;

    if (file == NULL) return NULL; //could not open file
    fseek(file, 0, SEEK_END);
    long f_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    code = malloc(f_size);

    while ((c = fgetc(file)) != EOF) {
        if (c == 10 || c == 32 || c == 9) {
            continue;
        }
        code[n++] = (char)c;
    }
    code[n] = '\0';
    return read_tree(code);
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

int sample(double * weights, int len) {
    double U = (double) rand()/RAND_MAX;
    U *= weights[len-1];
    return binsearch(weights, 0, len, U);
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

    printf("Reading a second time\n");
    struct TREE * tree2 = read_nwk_tree(argv[1]);
    int nleaves = tree2->nleaves;
    int nsamples = atoi(argv[2]);
    struct NODE * root2 = tree2->root;

    printf("Computing distances on tree (%d leaves)\n", nleaves);
    double * weights = (double*) malloc(sizeof(double)*nleaves);
    int i;
    for (i = 0; i < nleaves; i++)
        weights[i] = 0.0;
    leaf_distance(root2, weights);
    double * cum_weights = (double*) malloc(sizeof(double)*nleaves);
    double sum = 0.0;
    for (i = 0; i < nleaves; i++){
        sum += weights[i];
        cum_weights[i] = sum;
    }

    int * node_ids = (int*) malloc(nleaves*sizeof(int));
    for (i = 0; i < nleaves; i++){
        node_ids[i] = i;
    }
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

    printf("printing samples\n");
    for (i = rem_leaves; i < nleaves; i++){
        printf("%s\n", tree2->names[node_ids[i]]);
    }
}
