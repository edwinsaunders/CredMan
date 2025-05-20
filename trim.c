#include "cm-main.h"
// Trims leading and trailing whitespace from a string in place.
// Modifies the input string by shifting non-whitespace content to the start
// and null-terminating it after the last non-whitespace character.
void trim(char *str) {

    char *start = str;

    // loop until pointer 'start' points to first non-space char after pointer 'str'
        // start++ increments pointer address
    while (isspace((unsigned char)*start)) start++;

    // initialize 'end' to point to last char in string
    char *end = start + strlen(start) - 1;

    // decrement 'end' until it points to last non-space char
    while (end > start && isspace((unsigned char)*end)) end--;

    // put null-terminator after last non-space char
    *(end + 1) = '\0';

    // move new string pointed to by 'start' to the start of the 'str' buffer
    memmove(str, start, end - start + 2);
}