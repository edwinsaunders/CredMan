#include "cm-main.h"
// Saves credentials to a file.
// Writes each credential block to the specified file, separated by two newlines.
// Displays a confirmation message on success.
void save_credentials(const char *filename, Credential *creds, int num_creds) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Error saving file");
        return;
    }

    // Write each credential block
    for (int i = 0; i < num_creds; i++) {
        fprintf(file, "%s\n\n", creds[i].content);
    }

    fclose(file);
    // Display save confirmation
    printw("Credentials saved to %s\n", filename);
    refresh();
    sleep_ms(1000);
}