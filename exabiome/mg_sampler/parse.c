#include <stdio.h>
#include <stdlib.h>
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


NODE* read_node(char * nwk) {
    printf("nwk = %s\n", nwk);
    char *name;
    char *blen;
    size_t name_n = 0;
    size_t blen_n = 0;

    int read_blen = 0;

    name = malloc(100);
    blen = malloc(100);

    NODE * ret = (NODE *) malloc(sizeof(NODE));

    while (*nwk != '\0') {
        if (*nwk == '(') { // left parentheses -->
            nwk++;
            ret->left = read_node(nwk);
        }
        else if (*nwk == ')') { // right parentheses --> done reading branch length
            blen[blen_n] = '\0';
            ret->blen = atof(blen);
            nwk++;
            ret->right = read_node(nwk);
            break;
       } else if (*nwk == ','){ // comma --> add branch length to ret
            blen[blen_n] = '\0';
            ret->blen = atof(blen);
            nwk++;
            break;
        } else if (*nwk == ':') {  // colon --> finished reading contents of node, next up is branch length
            if (name_n > 0){   // we were reading a leaf node
                name[name_n] = '\0';
                nleaves++;
                ret->name = name;
            }
            read_blen = 1;
            nwk++;
        } else {
            if (read_blen) {
                blen[blen_n++] = *nwk;
            } else {
                name[name_n++] = *nwk;
            }
            nwk++;
        }
    }
    return ret;
}

void print_leaves(NODE* node){
    if (!node->name){
        printf("Found leaf ");
    } else {
        print_leaves(node->left);
        print_leaves(node->right);
    }
}

void print_nwk(char * nwk){
    char * tmp = nwk;
    while(*tmp){
        putchar(*tmp++);
    }
}

int main(){

    //FILE *file = fopen(fileName, "r");
    nleaves = 0;

    char * tree = "(C:1.0,(B:0.3,A:0.2):0.5)\0";
    print_nwk(tree);
    printf("\n");
    NODE * root = read_node(tree);
    print_leaves(root);

}

