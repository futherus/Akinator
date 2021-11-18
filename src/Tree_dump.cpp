#include "Tree_dump.h"

#include <string.h>
#include <stdlib.h>

static const int GRAPHVIZ_NAME_SIZE = 300;

static const char GRAPHVIZ_PNG_NAME[] = "tree_dump/graphviz_dump";
static const char TREE_DUMPFILE[]     = "tree_dump.html";

static const char GRAPHVIZ_INTRO[] =
R"(
digraph G{
    graph [dpi = 100];
    bgcolor = "#2F353B";
    ranksep = 1;
    edge[minlen = 3, arrowsize = 2, penwidth = 1.5, color = green];
    node[shape = rectangle, style = "filled, rounded", height = 2, width = 3,
         fixedsize = true, fillcolor = "#C5D0E6", fontsize = 30,
         color = "#C5D0E6", penwidth = 3];
)";

static const char GRAPHVIZ_OUTRO[] = "}\n";

static const char HTML_INTRO[] =
R"(
<html>
    <head>
        <title>
            Tree log
        </title>
        <style>
            .ok {color: springgreen;font-weight: bold;}
            .error{color: red;font-weight: bold;}
            .log{color: #C5D0E6;}
            .title{color: #E59E1F;text-align: center;font-weight: bold;}
        </style>
    </head>
    <body bgcolor = "#2F353B">
        <pre class = "log">
)";

static const char HTML_OUTRO[] =
R"(
        </pre>
    </body>
</html>
)";

static FILE* DUMP_STREAM = nullptr;
static FILE* TEMP_GRAPH_STREAM = nullptr;

static long DUMP_ITERATION = 0;

static char* graphviz_png_()
{
    static char filename[GRAPHVIZ_NAME_SIZE] = "";
    sprintf(filename, "%s_%ld.png", GRAPHVIZ_PNG_NAME, DUMP_ITERATION);

    return filename;
}

#define PRINT(format, ...) fprintf(stream, format, ##__VA_ARGS__)

static void tree_print_node_(Tree* tree, ptrdiff_t indx)
{
    FILE* stream = TEMP_GRAPH_STREAM;
    Node* node   = tree_get(tree, indx);

    PRINT("node%p[label = \"%s\"];\n", node, node->data);
    
    if(node->left  != INVLD_INDX)
        PRINT("node%p -> node%p;\n", node, tree_get(tree, node->left));
    if(node->right != INVLD_INDX)
        PRINT("node%p -> node%p;\n", node, tree_get(tree, node->right));
}

static void tree_graph_dump_(Tree* tree)
{
    FILE* stream = fopen("tree_dump/graphviz_temp.txt", "w");
    if(!stream)
    {
        perror("Can't open temporary dump file");
        return;
    }
    TEMP_GRAPH_STREAM = stream;

    PRINT("%s", GRAPHVIZ_INTRO);

    tree_visitor(tree, &tree_print_node_);

    PRINT("%s", GRAPHVIZ_OUTRO);

    fclose(stream);

    char sys_cmd[GRAPHVIZ_NAME_SIZE] = "dot tree_dump/graphviz_temp.txt -q -Tpng -o ";
    strcat(sys_cmd, graphviz_png_());

    system(sys_cmd);
}

static const char DATA_IS_NULL_MSG[] = "\n                                                                                            "
                                       "\n  DDDDDD      A    TTTTTTTTT    A         IIIII   SSSSS     N    N  U     U  L      L       "
                                       "\n  D     D    A A       T       A A          I    S          NN   N  U     U  L      L       "
                                       "\n  D     D   A   A      T      A   A         I     SSSS      N N  N  U     U  L      L       "
                                       "\n  D     D  AAAAAAA     T     AAAAAAA        I         S     N  N N  U     U  L      L       "
                                       "\n  DDDDDD  A       A    T    A       A     IIIII  SSSSS      N   NN   UUUUU   LLLLLL LLLLLL  "
                                       "\n                                                                                            ";

void tree_dump(Tree* tree, const char msg[], tree_err errcode)
{
    DUMP_ITERATION++;

    FILE* stream = DUMP_STREAM;
    if(stream == nullptr)
        return;

    PRINT("<span class = \"title\">----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------</span>\n");

    if(errcode)
        PRINT("<span class = \"title\"> %s (%d)</span>\n", msg, errcode);
    else    
        PRINT("<span class = \"title\"> %s </span>\n", msg);

    if(!tree)
    {
        PRINT("<span class = \"error\"> %s </span>\n", DATA_IS_NULL_MSG);
        PRINT("<span class = \"title\">\n----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n</span>");
        return;
    }

    PRINT("size: %lld\n" "capacity: %lld\n", tree->size, tree->cap);

    if(!tree->ptr_arr)
    {
        PRINT("<span class = \"error\"> %s </span>\n", DATA_IS_NULL_MSG);
        PRINT("<span class = \"title\">\n----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n</span>");
        return;
    }

    tree_graph_dump_(tree);

    PRINT(R"(<img src = ")" "%s" R"(" alt = "Graphical dump" width = 1080>)", graphviz_png_());

    PRINT("<span class = \"title\">\n----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n</span>");
}

#undef PRINT

static void close_dumpfile_()
{
    fprintf(DUMP_STREAM, "%s", HTML_OUTRO);

    if(fclose(DUMP_STREAM) != 0)
        perror("List dump file can't be succesfully closed");
}

void tree_dump_init(FILE* dumpstream)
{
    if(dumpstream)
    {
        DUMP_STREAM = dumpstream;
        return;
    }

    if(TREE_DUMPFILE[0] != 0)
    {
        DUMP_STREAM = fopen(TREE_DUMPFILE, "w");

        fprintf(DUMP_STREAM, "%s", HTML_INTRO);

        if(DUMP_STREAM)
        {
            atexit(&close_dumpfile_);
            return;
        }
    }

    perror("Can't open dump file");
    DUMP_STREAM = stderr;

    return;
}
