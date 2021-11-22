#include "Akinator.h"
#include "dumpsystem/dumpsystem.h"
#include "stack/include/Stack.h"
#include <stdio.h>
#include <sys/stat.h>
#include <ctype.h>
#include <assert.h>
#include <iconv.h>

static bool AKINATOR_QUIT_ = false;

#define IS_QUIT                             \
    {                                       \
        if(AKINATOR_QUIT_)                  \
            return AKNTR_NOERR;             \
    }                                       \

answer_type get_answer(char* answr_ptr)
{
    static char in_buffer[ANSWER_BUFF_SZ] = "";
    static char buffer   [ANSWER_BUFF_SZ] = "";
    char* in_buff_ptr   = in_buffer;
    char* buff_ptr      = buffer;
    size_t in_buff_len  = 0;
    size_t buff_space = ANSWER_BUFF_SZ;

    printf(">");
    
    gets(in_buffer);
    if(ferror(stdin))
        return ANSWER_EMPTY;
    
    in_buff_len = strlen(in_buffer);
    if(in_buff_len == 0)
        return ANSWER_EMPTY;

    iconv_t ptr = iconv_open("UTF-8", "CP1251");
    iconv(ptr, &in_buff_ptr, &in_buff_len, &buff_ptr, &buff_space);
    iconv_close(ptr);
    *buff_ptr = '\0';

    if(answr_ptr)
        memcpy(answr_ptr, buffer, ANSWER_BUFF_SZ - buff_space + 1);
            
    if(strcmp(buffer, "да")    == 0)
        return ANSWER_TRUE;
    if(strcmp(buffer, "нет")   == 0)
        return ANSWER_FALSE;
    if(strcmp(buffer, "отдыхать") == 0)
    {
        AKINATOR_QUIT_ = true;
        return ANSWER_QUIT;
    }
    
    return ANSWER_STRING;
}

static akinator_msg ask_(Node** node)
{
    printf("%s?\n", (*node)->data + 1);

    answer_type answr = ANSWER_EMPTY;
    while(true)
    {
        answr = get_answer();

        switch(answr)
        {
            case ANSWER_TRUE:
                *node = (*node)->right;
                return AKNTR_NOERR;
            case ANSWER_FALSE:
                *node = (*node)->left;
                return AKNTR_NOERR;
            case ANSWER_QUIT:
                printf("что-то быстро ты\n");
                return AKNTR_NOERR;
            case ANSWER_STRING : case ANSWER_EMPTY : default : 
            {/*fallthrough*/}
        }

        printf("некорректный ввод.\n"
               "введи 'да', 'нет' или 'выйти'\n");
    }

    ASSERT$(0, AKNTR_FALTHRGH, assert(0); );
    return AKNTR_FALTHRGH;
}

static akinator_msg add_answer_(Database* db, Node* node)
{
    ASSERT_RET$(db, AKNTR_NULLPTR);

    char temp_answer  [ANSWER_BUFF_SZ] = "";
    char temp_question[ANSWER_BUFF_SZ] = "";

    answer_type answer = ANSWER_EMPTY;

    printf("ну и кого ты там загадал?\n");

    while(true)
    {
        answer = get_answer(temp_answer);

        switch(answer)
        {
            case ANSWER_TRUE:
                printf("что \"да\"? напиши нормальный ответ\n");
                continue;
            case ANSWER_FALSE:
                printf("что \"нет\"? напиши нормальный ответ\n");
                continue;
            case ANSWER_QUIT:
                printf("а как же правильный ответ?...\n");
                return AKNTR_NOERR;
            case ANSWER_EMPTY:
                printf("ответ-то введи\n");
                continue;
            default:
                ASSERT_RET$(0, AKNTR_FALTHRGH);
            case ANSWER_STRING:
                {/*fallthrough*/}
        }

        break;
    }

    printf("а чем %s отличается от %s?\n", temp_answer, node->data);

    while(true)
    {
        answer = get_answer(temp_question + 1);

        switch(answer)
        {
            case ANSWER_TRUE:
                printf("что \"да\"? напиши нормальный ответ\n");
                continue;
            case ANSWER_FALSE:
                printf("что \"нет\"? напиши нормальный ответ\n");
                continue;
            case ANSWER_QUIT:
                printf("а как же правильный ответ?...\n");
                return AKNTR_NOERR;
            case ANSWER_EMPTY:
                printf("ответ-то введи\n");
                continue;
            default:
                ASSERT_RET$(0, AKNTR_FALTHRGH);
            case ANSWER_STRING:
                {/*fallthrough*/}
        }

        break;
    }

    temp_question[0] = '?';
    const char* answer_ptr   = nullptr;
    const char* question_ptr = nullptr;

    ASSERT_RET$(!array_add(&db->arr, &answer_ptr,   temp_answer) &&
                !array_add(&db->arr, &question_ptr, temp_question),
                AKNTR_ADD_ERR);

    ASSERT_RET$(!tree_add(&db->tree, &node->left,  node->data) &&
                !tree_add(&db->tree, &node->right, answer_ptr),
                AKNTR_ADD_ERR);
    
    node->data = question_ptr;
    
    return AKNTR_NOERR;
}

static akinator_msg guess_(Database* db, Node* node)
{
    ASSERT_RET$(db && node, AKNTR_NULLPTR);

    printf("это %s?\n", node->data);

    while(true)
    {
        answer_type answr = get_answer();
        switch(answr)
        {
            case ANSWER_TRUE:
                printf("а я еще и на машинке Тьюринга умею...\n");
                return AKNTR_NOERR;
            case ANSWER_QUIT:
                printf("страшно, да?\n");
                return AKNTR_NOERR;
            case ANSWER_FALSE:
                PASS$(!add_answer_(db, node), return AKNTR_ADD_ERR; );
                IS_QUIT;
                return AKNTR_NOERR;
            case ANSWER_EMPTY:
                printf("ответ-то введи\n");
                continue;
            case ANSWER_STRING : default :
                {/*fallthrough*/}
        }

        printf("некорректный ввод.\n"
               "введи 'да', 'нет' или 'выйти'\n");
    }

    ASSERT$(0, AKNTR_FALTHRGH, assert(0); );
    return AKNTR_FALTHRGH;
}

akinator_msg akinator_guesser(Database* db)
{
    ASSERT_RET$(db, AKNTR_NULLPTR);

    AKINATOR_QUIT_ = false;

    Node* node = db->tree.root;

    while(node->left || node->right)
    {
        PASS$(!ask_(&node), return AKNTR_GUESS_FAIL; );
        
        ASSERT_RET$(node, AKNTR_BAD_FORMAT);

        IS_QUIT;
    }

    ASSERT_RET$(!guess_(db, node), AKNTR_GUESS_FAIL);
    
    printf("\n");
    return AKNTR_NOERR;
}

static void describe_obj_(Node** path, int path_sz)
{    
    int iter = 0;
    while(iter < path_sz)
    {
        if(path[iter]->left == path[iter + 1])
            printf("не ");

        printf("%s", path[iter]->data + 1);

        if(iter == path_sz - 2)
            printf(" и ");
        if(iter < path_sz - 2)
            printf(", ");

        iter++;
    }
}

akinator_msg akinator_definition(Database* db)
{
    ASSERT_RET$(db, AKNTR_NULLPTR);

    char object[ANSWER_BUFF_SZ] = "";

    printf("кого описывать?\n");

    while(true)
    {
        answer_type answer = get_answer(object);

        switch(answer)
        {
            case ANSWER_TRUE:
                printf("что \"да\"? кого описывать-то?\n");
                continue;
            case ANSWER_FALSE:
                printf("что \"нет\"? кого описывать-то?\n");
                continue;
            case ANSWER_QUIT:
                printf("а я уже было все справочники достал\n");
                return AKNTR_NOERR;
            case ANSWER_EMPTY:
                printf("напиши кого описывать\n");
                continue;
            default:
                ASSERT_RET$(0, AKNTR_FALTHRGH);
            case ANSWER_STRING:
                {/*fallthrough*/}
        }

        break;
    }

    Stack path_stk = {};
    stack_init(&path_stk, 0);

    ASSERT_RET$(!tree_find(&db->tree, object, &path_stk), AKNTR_FIND_FAIL);

    if(path_stk.size == 0)
    {
        printf("не знаю такого\n");
        return AKNTR_NOERR;
    }

    Node** path = (Node**) path_stk.buffer;
    int path_sz = (int) path_stk.size - 1;

    printf("%s ", object);
    describe_obj_(path, path_sz);
    printf("\n\n");

    stack_dstr(&path_stk);

    return AKNTR_NOERR;
}

akinator_msg akinator_difference(Database* db)
{
    ASSERT_RET$(db, AKNTR_NULLPTR);

    char object1[ANSWER_BUFF_SZ] = "";
    char object2[ANSWER_BUFF_SZ] = "";

    printf("кого сравнить?\n");

    while(true)
    {
        answer_type answer = get_answer(object1);

        switch(answer)
        {
            case ANSWER_TRUE:
                printf("что \"да\"? сравнивать мне кого?\n");
                continue;
            case ANSWER_FALSE:
                printf("что \"нет\"? сравнивать мне кого?\n");
                continue;
            case ANSWER_QUIT:
                printf("без сравнения так без сравнения\n");
                return AKNTR_NOERR;
            case ANSWER_EMPTY:
                printf("напиши кого сравнить\n");
                continue;
            default:
                ASSERT_RET$(0, AKNTR_FALTHRGH);
            case ANSWER_STRING:
                {/*fallthrough*/}
        }

        break;
    }

    printf("а с кем?\n");

    while(true)
    {
        answer_type answer = get_answer(object2);

        switch(answer)
        {
            case ANSWER_TRUE:
                printf("что \"да\"? сравнивать мне кого?\n");
                continue;
            case ANSWER_FALSE:
                printf("что \"нет\"? сравнивать мне кого?\n");
                continue;
            case ANSWER_QUIT:
                printf("без сравнения так без сравнения\n");
                return AKNTR_NOERR;
            case ANSWER_EMPTY:
                printf("напиши кого сравнить\n");
                continue;
            default:
                ASSERT_RET$(0, AKNTR_FALTHRGH);
            case ANSWER_STRING:
                {/*fallthrough*/}
        }

        break;
    }

    if(strcmp(object1, object2) == 0)
    {
        printf("тут и сравнивать нечего, одинаковые же\n");
        return AKNTR_NOERR;
    }

    Stack stk_path1 = {};
    stack_init(&stk_path1, 0);
    ASSERT_RET$(!tree_find(&db->tree, object1, &stk_path1), AKNTR_FIND_FAIL);

    if(stk_path1.size == 0)
    {
        printf("не знаю %s\n", object1);
        return AKNTR_NOERR;
    }

    Stack stk_path2 = {};
    stack_init(&stk_path2, 0);
    ASSERT_RET$(!tree_find(&db->tree, object2, &stk_path2), AKNTR_FIND_FAIL);

    if(stk_path2.size == 0)
    {
        printf("не знаю %s\n", object2);
        return AKNTR_NOERR;
    }

    Node** path1 = (Node**) stk_path1.buffer;
    Node** path2 = (Node**) stk_path2.buffer;

    int iter = 0;
    int path1_sz = (int) stk_path1.size - 1;
    int path2_sz = (int) stk_path2.size - 1;

    while(path1[iter] == path2[iter] && path1[iter + 1] == path2[iter + 1])
        iter++;

    if(iter > 0)
    {
        printf("%s и %s похожи тем, что ", object1, object2);
        describe_obj_(path1, iter);
        printf(", но они отличаются тем, что ");
    }
    
    printf("%s ", object1);
    describe_obj_(path1 + iter, path1_sz - iter);

    printf(", тогда как ");

    printf("%s ", object2);
    describe_obj_(path2 + iter, path2_sz - iter);

    printf("\n\n");

    stack_dstr(&stk_path1);
    stack_dstr(&stk_path2);

    return AKNTR_NOERR;
}
