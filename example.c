#include <stdio.h>
#include "splay.h"

int compare_int(void *l, void *r) {
    return *((int*)l) - *((int*)r);
}

void output(splay_tree *tree) {
    splay_node *node = first(tree);
    while(node != NULL) {
        printf("%d ", *(int*)node->value);
        node = next(node);
    }
    printf("\n");
    void **t = contents(tree);
    for(int i = 0; i < tree->size; i++) {
        printf("%d ", *(int*)t[i]);
    }
    printf("\n");
}

int main() {
    splay_tree *tree = new_tree(&compare_int);
    int values[] = {3, 4, 1, 2, 8, 5, 7};
    output(tree);
    for(int i = 0; i < 7; i++) {
        insert(tree, &values[i]);
        printf("+%d: min %d max %d\n", values[i], 
                *(int*)first(tree)->value, *(int*)last(tree)->value);
        output(tree);
    }
    for(int i = 0; i < 7; i++) {
        delete(tree, &values[i]);
        printf("-%d: ", values[i]);
        if(first(tree) == NULL) {
            printf("EMPTY\n");
        } else {
            printf("min %d max %d\n",
                    *(int*)first(tree)->value, *(int*)last(tree)->value);
        }
        output(tree);
    }
    return 0;
}
