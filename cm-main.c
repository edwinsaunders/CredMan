#include "cm-main.h"


// Entry point for the credential management program.
// Reads credentials from a file, processes duplicates, handles same accounts,
// saves results, and provides an interactive query interface.
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <credential_file>\n", argv[0]);
        return 1;
    }

    // Initialize credential array
    int num_creds = 0;
    count_creds(argv[1], &num_creds);
    Credential *creds = malloc(num_creds * sizeof(Credential));

    if (!read_credentials(argv[1], creds, &num_creds)) {
        return 1;
    }
    
    initscr();
    cbreak();
    noecho();
    
    // Process credentials
    remove_duplicates(creds, &num_creds);
    printw("Removed exact duplicate credential blocks.\n");
    refresh();
    sleep_ms(1000);

    handle_same_accounts(creds, &num_creds);
    printw("Processed multiple entries for same accounts.\n");
    refresh();
    sleep_ms(1000);

    save_credentials("out.txt", creds, num_creds);
    query_credentials(creds, num_creds);
    free_credentials(creds, num_creds);

    return 0;
}