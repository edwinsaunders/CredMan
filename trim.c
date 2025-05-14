#include "cm-main.h"
// Trims leading and trailing whitespace from a string in place.
// Modifies the input string by shifting non-whitespace content to the start
// and null-terminating it after the last non-whitespace character.
void trim(char *str) {
    char *start = str;
    while (isspace((unsigned char)*start)) start++;
    char *end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char)*end)) end--;
    *(end + 1) = '\0';
    memmove(str, start, end - start + 2);
}