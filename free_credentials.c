#include "cm-main.h"
// Frees all dynamically allocated memory for the credential array.
// Releases memory for each credential's content and account strings,
// then frees the array itself.
void free_credentials(Credential *creds, int num_creds) {
    for (int i = 0; i < num_creds; i++) {
        free(creds[i].content);
        free(creds[i].account);
    }
    free(creds);
}