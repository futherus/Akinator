#include "Tree.h"
#include "Tree_dump.h"
#include "dumpsystem/dumpsystem.h"

#ifndef __USE_MINGW_ANSI_STDIO
#define __USE_MINGW_ANSI_STDIO 1
#endif
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define ASSERT(CONDITION, ERROR)                \
    do                                          \
    {                                           \
        if(!(CONDITION))                        \
        {                                       \
            tree_dump(tree, #ERROR, (ERROR));   \
            return (ERROR);                     \
        }                                       \
    } while(0)                                  \

static tree_err ptr_arr_resize_(Tree* tree)
{
    assert(tree);
    
    ptrdiff_t new_cap = tree->ptr_arr_cap;
    if(new_cap == 0)
        new_cap = TREE_PTR_ARR_MIN_CAP;

    Node** temp = (Node**) realloc(tree->ptr_arr, new_cap * sizeof(Node*));
    ASSERT(temp, TREE_BAD_ALLOC);
    
    memset(temp + tree->ptr_arr_cap, 0, new_cap - tree->ptr_arr_cap);

    tree->ptr_arr     = temp;
    tree->ptr_arr_cap = new_cap;
    
    return TREE_NOERR;
}

static tree_err add_chunk_(Tree* tree)
{
    assert(tree);

    if(tree->cap / TREE_CHUNK_SIZE == tree->ptr_arr_cap)
        if(ptr_arr_resize_(tree))
            return TREE_BAD_ALLOC;
    
    Node* temp = (Node*) calloc(TREE_CHUNK_SIZE, sizeof(Node));
    ASSERT(temp, TREE_BAD_ALLOC);

    tree->ptr_arr[tree->cap / TREE_CHUNK_SIZE] = temp;
    tree->cap += TREE_CHUNK_SIZE;
    
    return TREE_NOERR;
}

tree_err tree_init(Tree* tree, const char base_label[])
{
    ASSERT(tree, TREE_NULLPTR);
    ASSERT(!tree->ptr_arr, TREE_REINIT);

    if(add_chunk_(tree))
        return TREE_BAD_ALLOC;

    tree->ptr_arr[0][0] = {base_label, nullptr, nullptr};
    tree->size = 1;
    tree->root = &tree->ptr_arr[0][0];

    return TREE_NOERR;
}

tree_err tree_dstr(Tree* tree)
{
    ASSERT(tree, TREE_NULLPTR);
    ASSERT(tree->ptr_arr, TREE_NOTINIT);

    for(ptrdiff_t iter = 0; iter < tree->cap / TREE_CHUNK_SIZE; iter++)
        free(tree->ptr_arr[iter]);
    
    return TREE_NOERR;
}

tree_err tree_add(Tree* tree, Node** base_ptr, const char data[])
{
    ASSERT(tree && base_ptr && data, TREE_NULLPTR);
    ASSERT(tree->ptr_arr, TREE_NOTINIT);

    if(tree->cap == tree->size)
        if(add_chunk_(tree))
            return TREE_BAD_ALLOC;

    Node* new_node = &tree->ptr_arr[tree->size / TREE_CHUNK_SIZE][tree->size % TREE_CHUNK_SIZE];
    *new_node  = {data, nullptr, nullptr};
    *base_ptr  = new_node;

    tree->size++;

    return TREE_NOERR;
}

static void (*VISITOR_FUNCTION)(Tree*, Node*) = nullptr;

static void tree_visitor_(Tree* tree, Node* node)
{
    if(node->left)
        tree_visitor_(tree, node->left);
    if(node->right)
        tree_visitor_(tree, node->right);

    VISITOR_FUNCTION(tree, node);
}

tree_err tree_visitor(Tree* tree, void (*function)(Tree* tree, Node* node))
{
    ASSERT(tree && function, TREE_NULLPTR);
    ASSERT(tree->ptr_arr, TREE_NOTINIT);

    VISITOR_FUNCTION = function;

    tree_visitor_(tree, tree->root);

    return TREE_NOERR;
}
