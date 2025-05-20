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
                printf("\nMultiple entries found for account: %s\n", creds[i].account);
                printf("\nEntry %d:\n%s\n", i + 1, creds[i].content);
                printf("Entry %d:\n%s\n", j + 1, creds[j].content);
                printf("Choose an action:\n");
                printf("1. Keep first entry\n");
                printf("2. Keep second entry\n");
                printf("3. Rename entries\n");

                
                // Read user choice
                //set keypad mode TRUE to interpret escape seuqnces as ncurses keycodes
                // (arrow keys are escape sequnces: 27[A (octal 033[A), 27[B, etc)
                // keypad(stdscr, TRUE);
                

                int choice;
            label:
                // choice = 0;
                printf("\nEnter choice (1-3): ");

                scanf("%d", &choice);
                //flush input buffer so it wont loop due to reading a char from it instead
                    // of waiting for user input
                while ( getchar() != '\n' )
                    ;

                //printf("\n%d", choice);
               

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
                    continue;
                } else if (choice == 2) {
                    // Keep second entry, discard first
                    free(creds[i].content);
                    free(creds[i].account);
                    creds[i].content = NULL;
                    creds[i].account = NULL;
                    // break out of j loop to move on to next i val
                    break;
                } else if (choice == 3) {
                    // function to rename accounts should take current i and j creds
                    // and read from stdin to get new names, enter for no change
                    // move on to next j val to find other duplicates accounts for same i val
                    printf("Enter new name for entry %d: %s\nNew name: ", i + 1, creds[i].account);
                    rename_cred(creds[i].account);
                    printf("Enter new name for entry %d: %s\nNew name: ", j + 1, creds[j].account);
                    rename_cred(creds[j].account);
                    continue;
                } else {
                    //error msg and reprint prompt for invalid input
                    // move(cursor_y + 2, 0);
                    printf("\nInvalid input.  Please try again.\n");
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
    printf("DEBUG: Total credentials (after handle_same_accounts): %d\n", *num_creds);
}