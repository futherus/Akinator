#ifndef TREE_DUMP_H
#define TREE_DUMP_H

#include "Tree.h"

#ifndef __USE_MINGW_ANSI_STDIO
#define __USE_MINGW_ANSI_STDIO 1
#endif
#include <stdio.h>

void tree_dump_init(FILE* dumpstream);

void tree_dump(Tree* tree, const char msg[], tree_err errcode = TREE_NOERR);

#endif // TREE_DUMP_H