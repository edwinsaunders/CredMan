#include "cm-main.h"
// Compares two credential blocks for exact equality.
// Returns 1 if the blocks are identical, 0 otherwise.
int are_blocks_equal(const char *block1, const char *block2) {
    return strcmp(block1, block2) == 0;
}