#include "dumpsystem/dumpsystem.h"
#include "database.h"

#include <stdlib.h>
#include <sys/stat.h>
#include <ctype.h>

static int file_sz_(const char filename[], size_t* sz)
{
    struct stat buff = {};
    if(stat(filename, &buff) == -1)
        return -1;
    
    *sz = buff.st_size;
    
    return 0;
}

database_err database_read(Database* db, const char infilename[])
{
    ASSERT$(db && infilename,
            DATABASE_NULLPTR, return DATABASE_NULLPTR; )

    size_t file_sz = 0;
    ASSERT$(!file_sz_(infilename, &file_sz),
            DATABASE_READ_FAIL, return DATABASE_READ_FAIL; )
    
    ASSERT$(file_sz > 0,
            DATABASE_READ_EMPTY, return DATABASE_READ_EMPTY; )
    
    FILE* stream = fopen(infilename, "r");
    ASSERT$(stream,
           DATABASE_READ_FAIL, return DATABASE_READ_FAIL; )

    ASSERT$(!array_init(&db->arr, file_sz),
            DATABASE_BAD_ALLOC, return DATABASE_BAD_ALLOC; )
    
    size_t n_read = fread(db->arr.main_buffer, sizeof(char), file_sz, stream);

    if(n_read != file_sz)
        ASSERT$(!ferror(stream),
                DATABASE_READ_FAIL, return DATABASE_READ_FAIL; )

    return DATABASE_NOERR;
}

static char* tokenize_(char* ptr)
{
    static char* data = nullptr;

    if(!ptr && !data)
        return nullptr;
    if(ptr)
        data = ptr;

    printf("<%s>\n", data);

    char* frst_symb = nullptr;
    char* last_symb = nullptr;
    size_t iter = 0;

    while(isspace(data[iter]))
        iter++;
    
    frst_symb = &data[iter];

    while(data[iter] != '\n' && data[iter] != '\0')
    {
        if(!isspace(data[iter]))
            last_symb = &data[iter];

        iter++;
    }

    if(data[iter] == '\0')
    {
        data = nullptr;
        
        if(last_symb)
            return frst_symb;
        
        return nullptr;
    }

    data += iter + 1;
   *(last_symb + 1) = '\0';

    return frst_symb;
}

static Node* setup_node_(Tree* tree)
{
    char* data_ptr = tokenize_(nullptr);
    //ASSERT$(data_ptr,
    //        DATABASE_BAD_FORMAT, return INVLD_INDX; )
    if(!data_ptr)
        return nullptr;

    Node* node = nullptr;
    ASSERT$(!tree_add(tree, &node, data_ptr),
            DATABASE_TREE_FAIL, return nullptr; )

    if(data_ptr[0] != '?')
        return node;

    node->right = setup_node_(tree);
    node->left  = setup_node_(tree);

    //PASS$(node->right != INVLD_INDX && node->left != INVLD_INDX, 
    //      return INVLD_INDX; )

    return node;
}

database_err database_setup_tree(Database* db)
{
    ASSERT$(db,
            DATABASE_NULLPTR, return DATABASE_NULLPTR; )

    if(db->arr.main_buffer_sz == 0)
    {
        ASSERT$(!tree_init(&db->tree, TREE_DFLT_ROOT),
                DATABASE_TREE_FAIL, return DATABASE_TREE_FAIL; )

        return DATABASE_NOERR;
    }

    char* data_ptr = tokenize_(db->arr.main_buffer);
    ASSERT$(data_ptr,
            DATABASE_BAD_MAIN, return DATABASE_BAD_MAIN; )
    
    ASSERT$(!tree_init(&db->tree, data_ptr),
            DATABASE_TREE_FAIL, return DATABASE_TREE_FAIL; )

    db->tree.root->right = setup_node_(&db->tree);
    db->tree.root->left  = setup_node_(&db->tree);

    return DATABASE_NOERR;
}

database_err database_add_node(Database* db, Node** base_ptr, const char data[])
{
    ASSERT$(db && base_ptr && data, DATABASE_NULLPTR, return DATABASE_NULLPTR; )

    const char* ptr = nullptr;
    ASSERT$(!array_add(&db->arr, &ptr, data), DATABASE_ARR_ADD_FAIL, return DATABASE_ARR_ADD_FAIL; )

    ASSERT$(!tree_add(&db->tree, base_ptr, ptr), DATABASE_TREE_ADD_FAIL, return DATABASE_TREE_ADD_FAIL; )

    return DATABASE_NOERR;
}
