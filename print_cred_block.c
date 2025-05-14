#include "cm-main.h"
// Prints the account name and content of the third credential block (index 2).
// Used for debugging or specific output needs.
void print_cred_block(Credential *creds) {
    printf("%s\n%s\n", creds[2].account, creds[2].content);
}