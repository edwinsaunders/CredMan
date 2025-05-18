#include "cm-main.h"

size_t arr_length(char **arr) {
    size_t length = 0;
    while (arr[length] != NULL) {
        length++;
    }
    return length;
}

// Provides an interactive interface to query and display credentials.
// Uses ncurses to create input and results windows, supports fuzzy search,
// and allows navigation and selection of credentials.
void query_credentials(Credential *creds, int num_creds) {
    initscr();
    cbreak();
    noecho();
    refresh();
    keypad(stdscr, TRUE);

    // Get screen dimensions
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    // Create input and results windows
    WINDOW *input_win = newwin(3, max_x, 0, 0);
    box(input_win, 0, 0);
    mvwprintw(input_win, 1, 1, "Search: ");
    wrefresh(input_win);

    WINDOW *results_win = newwin(max_y - 3, max_x, 3, 0);

    //get height of results window to know how many results to display
    int results_max_y = getmaxy(results_win);

    // set num of results to display based on size of results window
    int num_results_to_display = results_max_y - 2;

    // Buffers and variables for user interaction
    char input[MAX_INPUT] = "";
    char **block_lines;
    char *content_copy;
    int num_lines = 0;
    int input_pos = 0;
    int selected = 0;
    int offset = 0;
    int *matches = malloc(num_creds * sizeof(int));
    if (!matches) {
        delwin(input_win);
        delwin(results_win);
        endwin();
        return;
    }
    int num_matches = 0;
    int *scores = malloc(num_creds * sizeof(int));
    int *match_scores = malloc(num_creds * sizeof(int));

    while (1) {
        box(input_win, 0, 0);
        mvwprintw(input_win, 1, 1, "Search: ");
        // Find matching credentials based on input
        num_matches = 0;
        for (int i = 0; i < num_creds; i++) {
            scores[i] = fuzzy_match(input, creds[i].account);
            if (input[0] == '\0' || scores[i] > 0) {
                matches[num_matches] = i;
                match_scores[num_matches] = scores[i];
                num_matches++;
            }
        }

        // sort matches by score using bubble sort
        //scores[N]
        // scores[matches[N]] goes with creds[N].account
        // def bubble_sort(arr):
        //     n = len(arr)
        //     for i in range(n):
        //         for j in range(0, n - i - 1):
        //             if arr[j] > arr[j + 1]:
        //                 arr[j], arr[j + 1] = arr[j + 1], arr[j]
        //     return arr

        int n = num_matches;
        int temp;
        int temp2;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n-i-1; j++) {
                if (match_scores[j+1] > match_scores[j]) {
                    temp = match_scores[j];
                    temp2 = matches[j];
                    match_scores[j] = match_scores[j+1];
                    matches[j] = matches[j+1];
                    match_scores[j+1] = temp;
                    matches[j+1] = temp2;
                }
            }
        }
        // Adjust selection and scrolling
        if (selected >= num_matches) selected = num_matches - 1;
        if (selected < 0) selected = 0;
        if (selected < offset) offset = selected;
        if (selected >= offset + num_results_to_display) offset = selected - num_results_to_display + 1;

        // Display results
        werase(results_win);
        box(results_win, 0, 0);
        for (int i = offset; i < num_matches && i < offset + num_results_to_display; i++) {
            if (i == selected) {
                wattron(results_win, A_REVERSE);
            }
            mvwprintw(results_win, i - offset + 1, 1, "%s", creds[matches[i]].account);
            if (i == selected) {
                wattroff(results_win, A_REVERSE);
            }
        }
        wrefresh(results_win);

        // Update input window
        mvwprintw(input_win, 1, 9, "%-*s", MAX_INPUT - 9, input);
        wmove(input_win, 1, 9 + input_pos);
        wrefresh(input_win);

        // Handle user input
        int ch = getch();


        if (ch == 27) {
            break;
        } else if (ch == '\n' && num_matches > 0) {
            werase(results_win);
            box(results_win, 0, 0);

            // original line
            // mvwprintw(results_win, 1, 1, "Credentials for %s:\n%s",
            //     creds[matches[selected]].account, creds[matches[selected]].content);
            
            // modified code
            content_copy = strdup(creds[matches[selected]].content);
            block_lines = string_split(content_copy);
            num_lines = arr_length(block_lines);
            mvwprintw(results_win, 1, 1, "Credentials for %s:", creds[matches[selected]].account);
            for(int j = 0; j < num_lines; j++) {
                mvwprintw(results_win, 3 + j, 1, "%s", block_lines[j]);                
            }
            
            // free memory
            for(int i = 0; i < num_lines; i++) {
                free(block_lines[i]);
            }
            free(block_lines);
            free(content_copy);



            mvwprintw(results_win, max_y - 6, 1, "Press any key to continue...");
            wrefresh(results_win);
            getch();
        // key codes needed to be modified KEY_UP, etc. did not work
        // 450 = KEY_UP, 456 = KEY_DOWN, 8 = KEY_BACKSPACE
        } else if (ch == KEY_UP && selected > 0) {
            selected--;
        } else if (ch == KEY_DOWN && selected < num_matches - 1) {
            selected++;
        } else if (ch == KEY_BACKSPACE || ch == 127) {
            if (input_pos > 0) {
                input[--input_pos] = '\0';
                offset = 0;
                selected = 0;
            }
        } else if (isprint(ch) && input_pos < MAX_INPUT - 1) {
            input[input_pos++] = ch;
            input[input_pos] = '\0';
            offset = 0;
            selected = 0;
        }
    }

    // Clean up
    free(matches);
    free(scores);
    free(match_scores);
    delwin(input_win);
    delwin(results_win);
    endwin();
}