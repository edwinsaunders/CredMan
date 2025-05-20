#include "cm-main.h"
// Extracts the account name from a credential block.
// Takes a credential block string, duplicates it, isolates the first line,
// trims whitespace, and returns the account name as a dynamically allocated string.
// Caller is responsible for freeing the returned string.
char *extract_account(const char *block) {
    char *account = strdup(block);
    char *newline = strchr(account, '\n');
    if (newline) *newline = '\0';
    //trim(account);
    return account;
}