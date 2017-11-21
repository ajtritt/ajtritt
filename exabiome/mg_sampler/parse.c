#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sample.h"



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

int nleaves;

NODE* read_node(char * nwk, int * idx) {
    char *name;
    char *blen;
    size_t name_n = 0;
    size_t blen_n = 0;

    int read_blen = 0;

    name = malloc(100);
    blen = malloc(100);

    NODE * ret = (NODE *) malloc(sizeof(NODE));

    while (nwk[*idx] != '\0') {
        if (nwk[*idx] == '(') { // left parentheses -->
            (*idx)++;
            ret->left = read_node(nwk, idx);
        }
        else if (nwk[*idx] == ')') { // right parentheses --> done reading branch length
            blen[blen_n] = '\0';
            ret->blen = atof(blen);
            read_blen = 0;
            (*idx)++;
            break;
       } else if (nwk[*idx] == ','){ // comma --> add branch length to ret
            if (read_blen){
                blen[blen_n] = '\0';
                ret->blen = atof(blen);
            } else {
                (*idx)++;
                ret->right = read_node(nwk, idx);
            }
            break;
        } else if (nwk[*idx] == ':') {  // colon --> finished reading contents of node, next up is branch length
            if (name_n > 0){   // we were reading a leaf node
                name[name_n] = '\0';
                nleaves++;
                ret->name = name;
            }
            read_blen = 1;
            (*idx)++;
        } else {
            if (read_blen) {
                blen[blen_n++] = nwk[*idx];
            } else {
                name[name_n++] = nwk[*idx];
            }
            (*idx)++;
        }
    }
    return ret;
}

void print_leaves(NODE* node){
    if (!node->left){
        printf("%s:%.3f\n", node->name, node->blen);
    } else {
        print_leaves(node->left);
        print_leaves(node->right);
    }
}

int main(){

    //FILE *file = fopen(fileName, "r");
    nleaves = 0;
    char * tree = (char*) malloc(26*sizeof(char));
    strcpy(tree, "(C:1.0,(B:0.3,A:0.2):0.5)\0");
    int idx = 0;
    int * idx_ptr = &idx;
    NODE * root = read_node(tree, idx_ptr);
    printf("Finished reading root, printing leaves\n");
    printf("Should print\nC:1.000\nB:0.300\nA:0.200\n");
    printf("----------\n");
    print_leaves(root);
    printf("----------\n");
    printf("should be C: %s\n", root->left->name);
    printf("should be B: %s\n", root->right->left->name);
    printf("should be A: %s\n", root->right->right->name);

}

