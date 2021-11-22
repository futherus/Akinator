#include <stdio.h>
#include "tree/Tree.h"
#include "Akinator.h"
#include "dumpsystem/dumpsystem.h"
#include "database.h"
#include <windows.h>

const char DEFAULT_DATAFILENAME[] = "akinator_database.txt";

int main(int argc, char* argv[])
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(65001);

    Database database = {};
    const char* datafilename = DEFAULT_DATAFILENAME;

    tree_dump_init(dumpsystem_get_stream(log));

    if(argc == 2)
    {
        datafilename = argv[1];
        PASS$(!database_read(&database, datafilename), return AKNTR_READ_FAIL; );
    }
    
    PASS$(!database_setup_tree(&database), return AKNTR_READ_FAIL; );
    tree_dump(&database.tree, "Dump");

    printf("Акинатор самый обыкновенный без изысков без МХАТовских пауз без задумчивого чесания головы сравнивает описывает можно поиграть можно объектов добавить только записать не забудь\n\n");
    
    char buffer[ANSWER_BUFF_SZ] = "";
    while(true)
    {
        printf("что делать?\n");

        get_answer(buffer);

        if(strcmp(buffer, "угадай") == 0)
        {
            PASS$(!akinator_guesser(&database), return AKNTR_GUESS_FAIL; );
        }
        else if(strcmp(buffer, "опиши") == 0)
        {
            PASS$(!akinator_definition(&database), return AKNTR_FIND_FAIL; );
        }
        else if(strcmp(buffer, "сравни") == 0)
        {
            PASS$(!akinator_difference(&database), return AKNTR_DIFF_FAIL; );
        }
        else if(strcmp(buffer, "отдыхать") == 0)
        {
            printf("базу данных сохранять?\n");

            answer_type answer = get_answer();
            while(answer != ANSWER_TRUE && answer != ANSWER_FALSE)
            {
                printf("введи 'да' или 'нет'\n");
                answer = get_answer();
            }

            if(answer == ANSWER_TRUE)
            {
                PASS$(!database_write(&database, datafilename), return AKNTR_WRITE_FAIL; );
                printf("сохранил\n");
            }

            printf("я спать...\n");
            break;
        }
        else
        {
            printf("тут вариантов всего три: 'угадай', 'опиши', 'сравни'\n"
                   "(ну и дать акинатору отдохнуть можно, 'отдыхать', собсна)\n\n");
        }
    }

    database_write(&database, "output.txt");
    database_dstr(&database);

    return 0;
}