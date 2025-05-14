#include "cm-main.h"
// Counts the number of credential blocks in a file.
// Reads the file, identifies blocks separated by empty lines, and updates num_creds.
// Handles file errors by printing to stderr.
void count_creds(const char *filename, int *num_creds) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return;
    }

    // Buffer for reading lines
    char buffer[MAX_LINE];
    bool first_line = true;

    // Parse file to count credential blocks
    primary:
        while (fgets(buffer, MAX_LINE, file)) {
            buffer[strcspn(buffer, "\r\n")] = '\0';
            
            if (first_line == true && strlen(buffer) == 0) {
                first_line = false;
                continue;
            } else if (strlen(buffer) == 0) {
                continue;
            } else {
                (*num_creds)++;
                goto alternate;
            }
        }

    alternate:
        while (fgets(buffer, MAX_LINE, file)) {
            buffer[strcspn(buffer, "\r\n")] = '\0';
            if (strlen(buffer)) {
                continue;
            } else {
                goto primary;
            }
        }

    fclose(file);
}