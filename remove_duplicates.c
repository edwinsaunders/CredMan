#include "cm-main.h"
// Removes duplicate credential blocks from the array.
// Compares blocks for exact equality, frees duplicates, and compacts the array.
// Updates num_creds to reflect the new count.
void remove_duplicates(Credential *creds, int *num_creds) {
    for (int i = 0; i < *num_creds; i++) {
        if (creds[i].content == NULL) continue;
        for (int j = i + 1; j < *num_creds; j++) {
            if (creds[j].content == NULL) continue;
            //strcmp returns 0 for equality
            if (!strcmp(creds[i].content, creds[j].content)) {
                // Free duplicate memory
                free(creds[j].content);
                free(creds[j].account);
                creds[j].content = NULL;
                creds[j].account = NULL;
            }
        }
    }

    // Compact the array to remove null entries
    int new_count = 0;
    for (int i = 0; i < *num_creds; i++) {
        if (creds[i].content != NULL) {
            creds[new_count] = creds[i];
            new_count++;
        }
    }
    *num_creds = new_count;
    printf("DEBUG: Total credentials (after remove_duplicates): %d\n", *num_creds);
}