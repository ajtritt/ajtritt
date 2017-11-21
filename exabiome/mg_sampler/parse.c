#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sample.h"


typedef struct TREE {
    NODE * root;
    int nleaves;
} TREE;

char *readFile(char *fileName) {
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
    return code;
}

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
    return ret;
}

void print_leaves(NODE * node){
    if (!node->left){
        printf("%s:%.3f\n", node->name, node->blen);
    } else {
        print_leaves(node->left);
        print_leaves(node->right);
    }
}

int main(){

    char * nwk = "(C:1.0,(B:0.3,A:0.2):0.5)\0";
    TREE * tree = read_node(&nwk);
    NODE * root = tree->root;
    printf("Finished reading root, printing leaves\n");
    printf("Should print\nC:1.000\nB:0.300\nA:0.200\n");
    printf("----------\n");
    print_leaves(root);
    printf("----------\n");
    printf("should be C: %s\n", root->left->name);
    printf("should be B: %s\n", root->right->left->name);
    printf("should be A: %s\n", root->right->right->name);

}

