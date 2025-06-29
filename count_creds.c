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

    /*
    alternate method:
    
    set no_data flag to 1
    read line
    if empty, set no_data flag to 1 and skip iteration
    else if no_data flag is 1, 
        increment num_creds and set no_data flag to 0
    */
    
    bool new_cred = false;
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

        // check if line is empty, if so, make sure new_cred flag is false
        if (strlen(buffer) == 0) {
            new_cred = false;
            // continue;
        } else if (new_cred == false) {
            (*num_creds)++;
            new_cred = true;
        }
    }

    fclose(file);
}