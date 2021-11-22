#include "dumpsystem/dumpsystem.h"
#include "database.h"

#include <stdlib.h>
#include <sys/stat.h>
#include <ctype.h>
#include <stdarg.h>

static const size_t SYS_CMD_SIZE    = 256;

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
    ASSERT_RET$(db && infilename, DATABASE_NULLPTR);

    size_t file_sz = 0;
    ASSERT_RET$(!file_sz_(infilename, &file_sz), DATABASE_READ_FAIL);
    
    ASSERT_RET$(file_sz > 0, DATABASE_READ_EMPTY);
    
    FILE* stream = fopen(infilename , "r");
    ASSERT_RET$(stream, DATABASE_READ_FAIL);

    ASSERT_RET$(!array_init(&db->arr, file_sz), DATABASE_ARRAY_FAIL);
    
    size_t n_read = fread(db->arr.main_buffer, sizeof(char), file_sz, stream);

    if(n_read != file_sz)
        ASSERT_RET$(!ferror(stream), DATABASE_READ_FAIL);
    
    ASSERT_RET$(!fclose(stream), DATABASE_READ_FAIL);

    return DATABASE_NOERR;
}

static FILE* DATABASE_WRITE_STREAM_ = nullptr;

static void write_node_(Node* node, size_t depth)
{
    for(size_t iter = 0; iter < depth; iter++)
        fprintf(DATABASE_WRITE_STREAM_, "    ");
    
    fprintf(DATABASE_WRITE_STREAM_, "%s\n", node->data);
}

database_err database_write(Database* db, const char outfilename[])
{
    ASSERT_RET$(db && outfilename, DATABASE_NULLPTR);

    FILE* stream = fopen(outfilename , "w");
    ASSERT_RET$(stream, DATABASE_WRITE_FAIL);
    DATABASE_WRITE_STREAM_ = stream;

    ASSERT_RET$(!tree_visitor(&db->tree, &write_node_), DATABASE_WRITE_FAIL);

    ASSERT_RET$(!fclose(stream), DATABASE_WRITE_FAIL);

    return DATABASE_NOERR;
}

database_err database_dstr(Database* db)
{
    ASSERT_RET$(!array_dstr(&db->arr), DATABASE_ARRAY_FAIL);

    ASSERT_RET$(!tree_dstr(&db->tree), DATABASE_TREE_FAIL);

    return DATABASE_NOERR;
}

static char* tokenize_(char* ptr)
{
    static char* data = nullptr;

    if(!ptr && !data)
        return nullptr;
    if(ptr)
        data = ptr;

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
            DATABASE_TREE_FAIL, return nullptr; );

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
    ASSERT_RET$(db, DATABASE_NULLPTR);

    if(db->arr.main_buffer_sz == 0)
    {
        ASSERT_RET$(!tree_init(&db->tree, TREE_DFLT_ROOT), DATABASE_TREE_FAIL);

        return DATABASE_NOERR;
    }

    char* data_ptr = tokenize_(db->arr.main_buffer);
    ASSERT_RET$(data_ptr, DATABASE_BAD_MAIN);
    
    ASSERT_RET$(!tree_init(&db->tree, data_ptr), DATABASE_TREE_FAIL);

    db->tree.root->right = setup_node_(&db->tree);
    db->tree.root->left  = setup_node_(&db->tree);

    return DATABASE_NOERR;
}
