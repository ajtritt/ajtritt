#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct NODE {
    struct NODE * left;
    struct NODE * right;
    int node_id;
    double blen;
    char* name;
} NODE;

typedef struct TREE {
    NODE * root;
    int nleaves;
    char ** names;
} TREE;

TREE * read_node(char ** nwk_ptr) {
    char * nwk = *nwk_ptr;

    char *name = malloc(256), *blen = malloc(256);
    size_t name_n = 0, blen_n = 0;

    NODE * root = (NODE *) malloc(sizeof(NODE));
    int nleaves = 0;

    int read_blen = 0;
    char c = *nwk;
    while (c != '\0') {
        if (c == '(') { // read left node
            nwk++;
            TREE * subtree = read_node(&nwk);
            root->left = subtree->root;
            nleaves += subtree->nleaves;
        }
        else if (c == ')') { // done reading branch length
            blen[blen_n] = '\0';
            root->blen = atof(blen);
            read_blen = 0;
            nwk++;
            break;
       } else if (c == ','){
            if (read_blen){ // add branch length to ret
                blen[blen_n] = '\0';
                root->blen = atof(blen);
            } else {       // read right node
                nwk++;
                TREE * subtree = read_node(&nwk);
                root->right = subtree->root;
                nleaves += subtree->nleaves;
            }
            break;
        } else if (c == ':') {  // read branch length
            if (name_n > 0){   // we were reading a leaf node
                name[name_n] = '\0';
                nleaves = 1;
                root->name = name;
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
    TREE * ret = (TREE *) malloc(sizeof(TREE));
    ret->root = root;
    ret->nleaves = nleaves;
    free(blen);
    return ret;
}

void add_ids_rec(NODE * node, char ** names, int * id){
    if (!node->left){
        names[*id] = node->name;
        node->node_id = *id;
        (*id)++;
    } else {
        node->node_id = -1;
        add_ids_rec(node->left, names, id);
        add_ids_rec(node->right, names, id);
    }
}

void add_ids(TREE * tree){
    int id_ctr = 0;
    printf("Adding IDs\n");
    printf("root id = %d\n", tree->root->node_id);
    add_ids_rec(tree->root, tree->names, &id_ctr);
}

TREE * read_tree(char * nwk){
    TREE * ret = read_node(&nwk);
    ret->names = (char**)malloc(ret->nleaves*sizeof(char*));
    add_ids(ret);
    return ret;
}

void print_leaves(NODE * node){
    if (!node->left){
        printf("%s:%.3f id = %d\n", node->name, node->blen, node->node_id);
    } else {
        print_leaves(node->left);
        print_leaves(node->right);
    }
}

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
        printf("tallying distances\n");
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

double * compute_total_distances(TREE * tree) {
    double * ret = (double *) malloc(tree->nleaves*sizeof(double));
    printf("Computing distances for %d leaves\n", tree->nleaves);
    int i;
    for (i = 0; i < tree->nleaves; i++){
        ret[i] = 0.0;
    }
    leaf_distance(tree->root, ret);
    return ret;
}


TREE * read_nwk_tree(char *fileName) {
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
    printf("Read the following newick string: %s\n", code);
    return read_tree(code);
}

int main(int argc, char ** argv) {

    if (argc != 2) {
        printf("Usage: sample_tree in.nwk\n");
        return -1;
    }

    printf("Found %d args\n", argc);
    TREE * tree = read_nwk_tree(argv[1]);
    printf("root id = %d\n", tree->root->node_id);
    print_leaves(tree->root);
    double * distances = compute_total_distances(tree);
    int i;
    for (i = 0; i < tree->nleaves; i++){
        printf("%s %.3f\n", tree->names[i], distances[i]);
    }
}
