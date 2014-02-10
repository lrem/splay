#ifndef SPLAY_H
#define SPLAY_H

typedef int (*comparator)(void *left, void *right);

typedef struct splay_node {
    struct splay_node *parent, *left, *right;
    void *value;
} splay_node;

typedef struct splay_tree {
    splay_node *root;
    comparator comp;
    int size;
} splay_tree;

extern splay_tree* new_tree(comparator comp);
extern splay_node* insert(splay_tree *tree, void *value);
extern splay_node* find(splay_tree *tree, void *value);
extern splay_node* first(splay_tree *tree);
extern splay_node* next(splay_node *node);
extern splay_node* last(splay_tree *tree);
extern void* contents(splay_tree *tree);
extern void delete(splay_tree *tree, void *value);
extern void delete_hint(splay_tree *tree, splay_node *node);

#endif
