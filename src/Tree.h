#ifndef TREE_H
#define TREE_H

#include <stdint.h>

const ptrdiff_t TREE_PTR_ARR_MIN_CAP = 4;
const ptrdiff_t TREE_CHUNK_SIZE      = 1024;
const ptrdiff_t INVLD_INDX           = -666;

struct Node
{
    const char* data = nullptr;

    ptrdiff_t left  = 0;
    ptrdiff_t right = 0;
};

struct Tree
{
    Node**    ptr_arr     = nullptr;
    ptrdiff_t ptr_arr_cap = 0;

    ptrdiff_t size = 0;
    ptrdiff_t cap  = 0;
};

enum tree_err
{
    TREE_NOERR = 0,
    TREE_NULLPTR = 1,
    TREE_BAD_ALLOC = 2,
    TREE_REINIT    = 3,
    TREE_NOTINIT   = 4,
};

tree_err tree_init(Tree* tree, const char base_label[]);
tree_err tree_dstr(Tree* tree);

Node* tree_get(Tree* tree, ptrdiff_t indx);

tree_err tree_add(Tree* tree, ptrdiff_t* new_indx, const char data[]);

tree_err tree_visitor(Tree* tree, void (*function)(Tree* tree, ptrdiff_t indx));

#endif // TREE_H