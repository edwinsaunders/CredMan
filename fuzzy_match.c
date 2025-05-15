#include "cm-main.h"
// Performs fuzzy matching between a pattern and a string.
// Returns a score based on case-insensitive character matches, with higher scores
// for better matches. Returns -1 if the pattern cannot be fully matched.
int fuzzy_match(const char *pattern, const char *str) {
    int score = 0;
    int p_len = strlen(pattern);
    int s_len = strlen(str);
    int p_idx = 0, s_idx = 0;
    // loop until either end of pattern reached or end of compare string
    while (p_idx < p_len && s_idx < s_len) {
        if (tolower((unsigned char)pattern[p_idx]) == tolower((unsigned char)str[s_idx])) {
            score += 10;
            p_idx++;
        } else {
            score -= 1;
        }
        s_idx++;
    }
    // if the pattern is bigger than the compare string (the account name) by the end of the loop
    // that is, if we ran out of letters in the account name before running out of letters in the current pattern
    // this cant possibly match, so remove it from the matches by giving automatic -1 score
    if (p_idx < p_len) return -1;
    return score;
}