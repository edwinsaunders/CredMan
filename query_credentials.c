#include "cm-main.h"
// Provides an interactive interface to query and display credentials.
// Uses ncurses to create input and results windows, supports fuzzy search,
// and allows navigation and selection of credentials.
void query_credentials(Credential *creds, int num_creds) {
    initscr();
    cbreak();
    noecho();
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
    wrefresh(results_win);

    // Buffers and variables for user interaction
    char input[MAX_INPUT] = "";
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

    while (1) {
        // Find matching credentials based on input
        num_matches = 0;
        for (int i = 0; i < num_creds; i++) {
            if (input[0] == '\0' || fuzzy_match(input, creds[i].account) > 0) {
                matches[num_matches++] = i;
            }
        }

        // Adjust selection and scrolling
        if (selected >= num_matches) selected = num_matches - 1;
        if (selected < 0) selected = 0;
        if (selected < offset) offset = selected;
        if (selected >= offset + MAX_DISPLAY) offset = selected - MAX_DISPLAY + 1;

        // Update results window
        werase(results_win);
        box(results_win, 0, 0);
        for (int i = offset; i < num_matches && i < offset + MAX_DISPLAY; i++) {
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

        //Debug
        mvwprintw(input_win, 2, 1, "Key code: %d  ", ch); // Debug key code
        wrefresh(input_win);

        if (ch == 27) {
            break;
        } else if (ch == '\n' && num_matches > 0) {
            werase(results_win);
            box(results_win, 0, 0);
            mvwprintw(results_win, 1, 1, "Credentials for %s:\n%s\n", creds[matches[selected]].account,
                      creds[matches[selected]].content);
            mvwprintw(results_win, max_y - 6, 1, "Press any key to continue...");
            wrefresh(results_win);
            refresh();
            getch();
            wrefresh(input_win);
        // key codes needed to be modified KEY_UP, etc. did not work
        // 450 = KEY_UP, 456 = KEY_DOWN, 8 = KEY_BACKSPACE
        } else if (ch == KEY_UP && selected > 0) {
            selected--;
        } else if (ch == KEY_DOWN && selected < num_matches - 1) {
            selected++;
        } else if (ch == KEY_BACKSPACE || ch == 127) {
            if (input_pos > 0) {
                input[--input_pos] = '\0';
            }
        } else if (isprint(ch) && input_pos < MAX_INPUT - 1) {
            input[input_pos++] = ch;
            input[input_pos] = '\0';
        }
    }

    // Clean up
    free(matches);
    delwin(input_win);
    delwin(results_win);
    endwin();
}