#include <stdlib.h>
#include <assert.h>
#include "splay.h"

void check_sanity(splay_tree *tree);

/* The single most important utility function. */
void rotate(splay_node *child);
/* And a few more. */
splay_node* leftmost(splay_node *node);
splay_node* rightmost(splay_node *node);


/* The meat: splay the node x. */
void zig(splay_node *x, splay_node *p);
void zigzig(splay_node *x, splay_node *p);
void zigzag(splay_node *x, splay_node *p);
void splay(splay_tree *tree, splay_node *x) {
    while(1) {
        splay_node *p = x->parent;
        if(p == NULL) {
            tree->root = x;
            return;
        }
        splay_node *g = p->parent;
        if(p->parent == NULL)
            zig(x, p);
        else
            if((x == p->left && p == g->left) ||
                    (x == p->right && p == g->right))
                zigzig(x, p);
            else
                zigzag(x, p);
    }
}

/* When p is root, rotate on the edge between x and p.*/
void zig(splay_node *x, splay_node *p) {
    rotate(x);
}

/* When both x and p are left (or both right) children,
 * rotate on edge between p and g, then on edge between x and p.
 */
void zigzig(splay_node *x, splay_node *p) {
    rotate(p);
    rotate(x);
}

/* When one of x and p is a left child and the other a right child,
 * rotate on the edge between x and p, then on the new edge between x and g.
 */
void zigzag(splay_node *x, splay_node *p) {
    rotate(x);
    rotate(x);
}

/* Return an empty tree, storing the comparator. */
extern splay_tree* new_tree(comparator comp) {
    splay_tree *new = malloc(sizeof(splay_tree));
    new->comp = comp;
    new->root = NULL;
    new->size = 0;
    return new;
}

/* Insert and return a new node with the given value, splaying the tree. 
 * The insertion is essentially a generic BST insertion.
 */
extern splay_node* insert(splay_tree *tree, void *value) {
    splay_node *new = malloc(sizeof(splay_node));
    new->value = value;
    new->left = NULL;
    new->right = NULL;
    if(tree->root == NULL) {
        tree->root = new;
        new->parent = NULL;
    } else {
        splay_node *curr = tree->root;
        splay_node *parent;
        int left;
        while(curr != NULL) {
            parent = curr;
            if(tree->comp(new->value, curr->value) < 0) {
                left = 1;
                curr = curr->left;
            } else {
                left = 0;
                curr = curr->right;
            }
        }
        new->parent = parent;
        if(left)
            parent->left = new;
        else
            parent->right = new;
    }
    splay(tree, new);
    tree->size++;
    return new;
}

/* Find a node with the given value, splaying the tree. */
extern splay_node* find(splay_tree *tree, void *value) {
    splay_node *curr = tree->root;
    int found = 0;
    while(curr != NULL && !found) {
        int relation = tree->comp(value, curr->value);
        if(relation == 0) {
            found = 1;
        } else if(relation < 0) {
            curr = curr->left;
        } else {
            curr = curr->right;
        }
    }
    if(curr != NULL)
        splay(tree, curr);
    return curr;
}

/* Remove a node with the given value, splaying the tree. */
extern void delete(splay_tree *tree, void *value) {
    splay_node *node = find(tree, value);
    delete_hint(tree, node);
}

/* Remove the node given by the pointer, splaying the tree. */
extern void delete_hint(splay_tree *tree, splay_node *node) {
    if(node == NULL)
        return;
    splay(tree, node); /* Now node is tree's root. */
    if(node->left == NULL) {
        tree->root = node->right;
        if(tree->root != NULL)
            tree->root->parent = NULL;
    } else if(node->right == NULL) {
        tree->root = node->left;
        tree->root->parent = NULL;
    } else {
        splay_node *x = leftmost(node->right);
        if(x->parent != node) {
            x->parent->left = x->right;
            if(x->right != NULL)
                x->right->parent = x->parent;
            x->right = node->right;
            x->right->parent = x;
        }
        tree->root = x;
        x->parent = NULL;
        x->left = node->left;
        x->left->parent = x;
    }
    free(node);
    tree->size--;
    check_sanity(tree);
}

extern splay_node* first(splay_tree *tree) {
    return leftmost(tree->root);
}

/* Return the minimal node that is bigger than the given.
 * This is either:
 *  - leftmost child in the right subtree
 *  - closest ascendant for which given node is in left subtree
 */
extern splay_node* next(splay_node *node) {
    if(node->right != NULL)
        return leftmost(node->right);
    while(node->parent != NULL && node == node->parent->right)
        node = node->parent;
    return node->parent;
}

extern splay_node* last(splay_tree *tree) {
    return rightmost(tree->root);
}

/* An in-order traversal of the tree. */
void store(splay_node *node, void ***out);
extern void* contents(splay_tree *tree) {
    if(tree->size == 0)
        return NULL;
    void **out = malloc(tree->size * sizeof(void*));
    void ***tmp = &out;
    store(tree->root, tmp);
    return out - tree->size;
}

void store(splay_node *node, void ***out) {
    if(node->left != NULL)
        store(node->left, out);
    **out = node->value;
    (*out)++;
    if(node->right != NULL)
        store(node->right, out);
}
/* This mutates the parental relationships, copy pointer to old parent. */
void mark_gp(splay_node *child);

/* Rotate to make the given child take its parent's place in the tree. */
void rotate(splay_node *child) {
    splay_node *parent = child->parent;
    assert(parent != NULL);
    if(parent->left == child) { /* A left child given. */
        mark_gp(child);
        parent->left = child->right;
        if(child->right != NULL)
            child->right->parent = parent;
        child->right = parent;
    } else { /* A right child given. */ 
        mark_gp(child);
        parent->right = child->left;
        if(child->left != NULL)
            child->left->parent = parent;
        child->left = parent;
    }
}

void mark_gp(splay_node *child) {
    splay_node *parent = child->parent;
    splay_node *grand = parent->parent;
    child->parent = grand;
    parent->parent = child;
    if(grand == NULL)
        return;
    if(grand->left == parent)
        grand->left = child;
    else
        grand->right = child;
}

splay_node* leftmost(splay_node *node) {
    splay_node *parent = NULL;
    while(node != NULL) {
        parent = node;
        node = node->left;
    }
    return parent;
}

splay_node* rightmost(splay_node *node) {
    splay_node *parent = NULL;
    while(node != NULL) {
        parent = node;
        node = node->right;
    }
    return parent;
}

#ifdef DEBUG
int check_node_sanity(splay_node *x, void *floor, void *ceil, comparator comp) {
    int count = 1;
    if(x->left != NULL) {
        assert(x->left->parent == x);
        void *new_floor;
        if(floor == NULL || comp(x->value, floor) < 0)
            new_floor = x->value;
        else
            new_floor = floor;
        count += check_node_sanity(x->left, new_floor, ceil, comp);
    }
    if(x->right != NULL) {
        assert(x->right->parent == x);
        void *new_ceil;
        if(ceil == NULL || comp(x->value, ceil) > 0)
            new_ceil = x->value;
        else
            new_ceil = ceil;
        count += check_node_sanity(x->right, floor, new_ceil, comp);
    }
    return count;
}
#endif

void check_sanity(splay_tree *tree) {
#ifdef DEBUG
    if(tree->root == NULL) {
        assert(tree->size == 0);
    } else {
        int reachable = check_node_sanity(tree->root, NULL, NULL, tree->comp);
        assert(reachable == tree->size);
    }
#endif
}
