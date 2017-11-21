#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sample.h"

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
    return read_tree(code);
}

int main(int argc, char ** argv) {

    if (argc != 2) {
        printf("Usage: sample_tree in.nwk\n");
        return -1;
    }

    TREE * root = read_nwk_tree(argv[1]);
    double * distances = compute_total_distances(root);
    int i;
    for (i = 0; i < root->nleaves; i++){
        printf("%s %.3f\n", root->names[i], distances[i]);
    }
}
