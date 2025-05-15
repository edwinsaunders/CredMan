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



    // Parse file to count credential blocks
    primary:
        while (fgets(buffer, MAX_LINE, file)) {
            buffer[strcspn(buffer, "\r\n")] = '\0';
            
            // loop through buffer, check for whitespace, stop as soon as non-whitespace found
            // trim buffer if it is all whitespace chars
            int i = 0;
            while (isspace(buffer[i]) && i < strlen(buffer)) {
                i++;            
            }
            if (i == strlen(buffer)) {
                trim(buffer);
            }
            // check if line is empty
            if (strlen(buffer) == 0) {
                continue;
            } else {
                (*num_creds)++;
                goto alternate;
            }
        }

    alternate:
        while (fgets(buffer, MAX_LINE, file)) {
            buffer[strcspn(buffer, "\r\n")] = '\0';

            // loop through buffer, check for whitespace, stop as soon as non-whitespace found
            // trim buffer if it is all whitespace chars
            int i = 0;
            while (isspace(buffer[i]) && i < strlen(buffer)) {
                i++;            
            }
            if (i == strlen(buffer)) {
                trim(buffer);
            }
            //check if line is NOT empty
            if (strlen(buffer)) {
                continue;
            } else {
                goto primary;
            }
        }

    fclose(file);
}