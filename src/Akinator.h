#ifndef AKINATOR_H
#define AKINATOR_H

#include "database.h"

enum akinator_msg
{
    AKNTR_NOERR      = 0,
    AKNTR_NULLPTR    = 1,
    AKNTR_ADD_ERR    = 2,
    AKNTR_FALTHRGH   = 3,
    AKNTR_READ_FAIL  = 4,
    AKNTR_BAD_FORMAT = 5,
    AKNTR_GUESS_FAIL = 6,
    AKNTR_FIND_FAIL  = 7,
    AKNTR_DIFF_FAIL  = 8,
    AKNTR_WRITE_FAIL = 9,
    AKNTR_QUIT       = 77,
    AKNTR_FALSE      = 110,
    AKNTR_TRUE       = 111,
};

enum answer_type
{
    ANSWER_EMPTY  = 0,
    ANSWER_FALSE  = 110,
    ANSWER_TRUE   = 111,
    ANSWER_STRING = 112,
    ANSWER_QUIT   = 113,
};

const size_t ANSWER_BUFF_SZ = 256;
answer_type get_answer(char* answr_ptr = nullptr);

akinator_msg akinator_guesser(Database* db);
akinator_msg akinator_definition(Database* db);
akinator_msg akinator_difference(Database* db);

#endif // AKINATOR_H