#include "cm-main.h"
// Handles multiple credential entries for the same account.
// Prompts the user to choose which entry to keep (first, second, or both)
// and frees memory for discarded entries. Updates num_creds accordingly.
void handle_same_accounts(Credential *creds, int *num_creds) {
    scrollok(stdscr, TRUE);
    for (int i = 0; i < *num_creds; i++) {
        if (creds[i].content == NULL) continue;
        for (int j = i + 1; j < *num_creds; j++) {
            if (creds[j].content == NULL) continue;
            if (same_account(creds[i].account, creds[j].account)) {
                // Display duplicate account entries
                clear();
                move(0, 0);
                printw("\nMultiple entries found for account: %s\n", creds[i].account);
                printw("\nEntry %d:\n%s\n", i + 1, creds[i].content);
                printw("Entry %d:\n%s\n", j + 1, creds[j].content);
                printw("Choose an action:\n");
                printw("1. Keep first entry\n");
                printw("2. Keep second entry\n");
                printw("3. Keep both entries\n");
                refresh();

                

                // Read user choice
                //set keypad mode TRUE to interpret escape seuqnces as ncurses keycodes
                // (arrow keys are escape sequnces: 27[A (octal 033[A), 27[B, etc)
                // keypad(stdscr, TRUE);
                int choice;
                echo();

                label:

                printw("\nEnter choice (1-3): ");
                scanw("%d", &choice);

                //printw("%d", choice);
                //refresh();
                //logic for handling user choice (keycodes 49, 50, 27, 10 are 1, 2, ESC and ENTER respectively)
                if (choice == 1) {
                    // Keep first entry, discard second
                    free(creds[j].content);
                    free(creds[j].account);
                    creds[j].content = NULL;
                    creds[j].account = NULL;
                    // no break statement, keep checking on current i value
                } else if (choice == 2) {
                    // Keep second entry, discard first
                    free(creds[i].content);
                    free(creds[i].account);
                    creds[i].content = NULL;
                    creds[i].account = NULL;
                    // break out of j loop to move on to next i value
                    break;
                } else if (choice == 3) {
                    break;
                } else {
                    //error msg and reprint prompt for invalid input
                    // move(cursor_y + 2, 0);
                    printw("Invalid input.  Please try again.\n");
                    // move(cursor_y, 0);
                    goto label;
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