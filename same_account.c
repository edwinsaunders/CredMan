#include "cm-main.h"
// Compares two account names case-insensitively.
// Returns 1 if the account names match (ignoring case), 0 otherwise.
int same_account(const char *account1, const char *account2) {
    return strcasecmp(account1, account2) == 0;
}