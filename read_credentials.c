#include "cm-main.h"
// Reads credential blocks from a file into a Credential array.
// Parses the file, allocates memory for each block and account name,
// and stores them in the creds array. Updates num_creds and returns 1 on success, 0 on failure.
int read_credentials(const char *filename, Credential *creds, int *num_creds) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Error opening file");
        return 0;
    }

    // Buffers for reading and building credential blocks
    char buffer[MAX_LINE];
    char current_block[MAX_BLOCK_SIZE] = "";
    char *temp;
    int block_size = 0;
    int empty_lines = 0;
    int cred_count = 0;

    // Read file line by line
    while (fgets(buffer, MAX_LINE, file)) {
        buffer[strcspn(buffer, "\r\n")] = '\0';

        // loop through buffer, check for whitespace, stop as soon as non-whitespace found
            // trim buffer if it is all whitespace chars
            // also check for non-printable chars and replace with empty space
        int num_spaces = 0;
        int i = 0;
        while (i < strlen(buffer)) {
            if (isspace(buffer[i])) {
                num_spaces++;
            // convert non printable quotes and apostrophe to printable single quote
            } else if (buffer[i] == '\222' || buffer[i] == '\223' || buffer[i] == '\224') {
                buffer[i] = '\x27';
            } else if (!isprint(buffer[i])) {
                buffer[i] = ' ';
            }

            i++;
        }

        // trim leading and trailing whitespace char
        trim(buffer);
         
        if (strlen(buffer) == 0) {
            empty_lines++;
            if (empty_lines >= 1 && block_size > 0) {
                // Finalize current block
                current_block[block_size] = '\0';
                //creds[cred_count].content = strdup(current_block);
                memcpy(creds[cred_count].content, current_block, block_size + 1);
                //creds[cred_count].account = extract_account(current_block);
                temp = extract_account(current_block);
                memcpy(creds[cred_count].account, temp, strlen(temp) + 1);
                //free temp buffer
                free(temp);
                
                cred_count++;


                current_block[0] = '\0';
                block_size = 0;
            }
        } else {
            empty_lines = 0;
            if (block_size + strlen(buffer) + 2 < MAX_BLOCK_SIZE) {
                // Append line to current block with newline
                block_size += sprintf(current_block + block_size, "%s\n", buffer);
            }
        }
    }

    // Handle any remaining block
    if (block_size > 0) {
        current_block[block_size] = '\0';
        //creds[cred_count].content = strdup(current_block);
        memcpy(creds[cred_count].content, current_block, block_size + 1);
        //creds[cred_count].account = extract_account(current_block);
        temp = extract_account(current_block);
        memcpy(creds[cred_count].account, temp, strlen(temp) + 1);
        free(temp);
    }

    fclose(file);
    printf("DEBUG: Total credentials loaded (before handle_same_accounts): %d\n", *num_creds);
    printf("DEBUG: cred_count in read_credentials): %d\n", cred_count);
    return 1;
}