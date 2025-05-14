#include "cm-main.h"
// Handles multiple credential entries for the same account.
// Prompts the user to choose which entry to keep (first, second, or both)
// and frees memory for discarded entries. Updates num_creds accordingly.
void handle_same_accounts(Credential *creds, int *num_creds) {
    for (int i = 0; i < *num_creds; i++) {
        if (creds[i].content == NULL) continue;
        for (int j = i + 1; j < *num_creds; j++) {
            if (creds[j].content == NULL) continue;
            if (same_account(creds[i].account, creds[j].account)) {
                // Display duplicate account entries
                move(0, 0);
                printw("\nMultiple entries found for account: %s\n", creds[i].account);
                printw("Entry %d:\n%s\n", i + 1, creds[i].content);
                printw("Entry %d:\n%s\n", j + 1, creds[j].content);
                printw("Choose an action:\n");
                printw("1. Keep first entry\n");
                printw("2. Keep second entry\n");
                printw("3. Keep both entries\n");
                printw("Enter choice (1-3): ");
                refresh();

                // Read user choice
                int choice;
                scanw("%d", &choice);

                if (choice == 1) {
                    // Keep first entry, discard second
                    free(creds[j].content);
                    free(creds[j].account);
                    creds[j].content = NULL;
                    creds[j].account = NULL;
                } else if (choice == 2) {
                    // Keep second entry, discard first
                    free(creds[i].content);
                    free(creds[i].account);
                    creds[i].content = NULL;
                    creds[i].account = NULL;
                    break;
                }
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
}