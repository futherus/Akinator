#ifndef DATABASE_H
#define DATABASE_H

#include "Tree.h"
#include "array/Array.h"

const char TREE_DFLT_ROOT[] = "Неизвестно кто";
const size_t DATABASE_BUFF_MIN_SZ = 256;

struct Database
{
    Array arr = {};

    Tree tree = {};
};

enum database_err
{
    DATABASE_NOERR         = 0,
    DATABASE_NULLPTR       = 1,
    DATABASE_OVRREAD       = 2,
    DATABASE_READ_FAIL     = 3,
    DATABASE_READ_EMPTY    = 4,
    DATABASE_BAD_ALLOC     = 5,
    DATABASE_BAD_FORMAT    = 6,
    DATABASE_TREE_FAIL     = 7,
    DATABASE_BAD_MAIN      = 8,
    DATABASE_ARR_ADD_FAIL  = 9,
    DATABASE_TREE_ADD_FAIL = 10,
};

database_err database_read(Database* db, const char infilename[]);

database_err database_setup_tree(Database* db);

database_err database_add_node(Database* db, Node** base_ptr, const char data[]);

#endif // DATABASE_H
