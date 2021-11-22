#ifndef TREE_H
#define TREE_H

#include <stdint.h>
#include <stdio.h>
#include "../stack/include/Stack.h"

const ptrdiff_t TREE_PTR_ARR_MIN_CAP = 8;
const ptrdiff_t TREE_CHUNK_SIZE      = 512;

struct Node
{
    const char* data = nullptr;

    Node* left  = 0;
    Node* right = 0;
};

struct Tree
{
    Node**    ptr_arr     = nullptr;
    ptrdiff_t ptr_arr_cap = 0;

    ptrdiff_t size = 0;
    ptrdiff_t cap  = 0;

    Node* root = nullptr;
};

enum tree_err
{
    TREE_NOERR      = 0,
    TREE_NULLPTR    = 1,
    TREE_BAD_ALLOC  = 2,
    TREE_REINIT     = 3,
    TREE_NOTINIT    = 4,
    TREE_STACK_FAIL = 5,
};

tree_err tree_init(Tree* tree, const char root_label[]);
tree_err tree_dstr(Tree* tree);

tree_err tree_add(Tree* tree, Node** base_ptr, const char* data_ptr);

tree_err tree_visitor(Tree* tree, void (*function)(Node* node, size_t depth));
tree_err tree_find(Tree* tree, const char data[], Stack* path_stk);

void     tree_dump_init(FILE* dumpstream);

void     tree_dump(Tree* tree, const char msg[], tree_err errcode = TREE_NOERR);

#endif // TREE_H