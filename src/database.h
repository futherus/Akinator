#ifndef DATABASE_H
#define DATABASE_H

#include "tree/Tree.h"
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
    DATABASE_NOERR         =  0,
    DATABASE_NULLPTR       =  1,
    DATABASE_OVRREAD       =  2,
    DATABASE_READ_FAIL     =  3,
    DATABASE_READ_EMPTY    =  4,
    DATABASE_BAD_ALLOC     =  5,
    DATABASE_BAD_FORMAT    =  6,
    DATABASE_TREE_FAIL     =  7,
    DATABASE_ARRAY_FAIL    =  8,
    DATABASE_BAD_MAIN      =  9,
    DATABASE_WRITE_FAIL    = 10,
};

database_err database_read(Database* db, const char infilename[]);
database_err database_write(Database* db, const char outfilename[]);
database_err database_dstr(Database* db);

database_err database_setup_tree(Database* db);

database_err database_add_node(Database* db, Node** base_ptr, const char data[]);

#endif // DATABASE_H
