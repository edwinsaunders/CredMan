#ifndef CM_MAIN_H
#define CM_MAIN_H

#ifdef _WIN32
#include "/home/edwin/Downloads/PDCurses-3.9/curses.h"
#include "/usr/x86_64-w64-mingw32/include/windows.h"
#define sleep_ms(ms) Sleep(ms)
#define strcasecmp _stricmp
// #define KEY_UP 450
// #define KEY_DOWN 456
#define KEY_BACKSPACE 8
// #include <string.h>
#ifndef strdup
#define strdup _strdup
#endif
#else
#include <ncurses.h>
#include <unistd.h>
// #include <strings.h>
#define sleep_ms(ms) usleep((ms) * 1000)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
// #include <ncurses.h>
// #include <unistd.h>
#include <stdbool.h>

#define MAX_BLOCK_SIZE 4096
// #define MAX_BLOCKS 100
#define MAX_LINE 1024
#define MAX_INPUT 256
// #define MAX_DISPLAY 20
#define DEFAULT_FILE "out.txt"

typedef struct {
    char *content;
    char *account;
} Credential;

void trim(char *str);
char *extract_account(const char *block);
int are_blocks_equal(const char *block1, const char *block2);
int same_account(const char *account1, const char *account2);
int fuzzy_match(const char *pattern, const char *str);
void print_cred_block(Credential *creds);
void count_creds(const char *filename, int *num_creds);
int read_credentials(const char *filename, Credential *creds, int *num_creds);
void remove_duplicates(Credential *creds, int *num_creds);
void handle_same_accounts(Credential *creds, int *num_creds);
void save_credentials(const char *filename, Credential *creds, int num_creds);
void query_credentials(Credential *creds, int num_creds);
void free_credentials(Credential *creds, int num_creds);
char **string_split(char *string);
void rename_cred(char *string);


#endif // CM_MAIN_H