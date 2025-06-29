#include "cm-main.h"

#ifdef _WIN32
void _set_fixed_console_size(int rows, int cols) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD size = {cols, rows};
    SMALL_RECT rect = {0, 0, cols - 1, rows - 1};

    // Set console buffer size
    SetConsoleScreenBufferSize(hConsole, size);

    // Set window size
    SetConsoleWindowInfo(hConsole, TRUE, &rect);

    // Disable resizing and maximize button
    HWND hwnd = GetConsoleWindow();
    SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX);
    SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

    // Update pdcurses
    resize_term(rows, cols);
}
#endif

WINDOW *input_win, *results_win, *bg_win;

// Calculate lines occupied by a string, including wrapping
int _calc_lines(const char *str) {
    int len = strlen(str);
    return (len + DISPLAY_WIDTH - 1) / DISPLAY_WIDTH;
}

// Print string and return next empty y-position
int _print_line(WINDOW *win, int start_y, int x, const char *str) {
    mvwprintw(win, start_y, x, "%s", str);
    return start_y + _calc_lines(str);
}

void _redraw_ui() {

    endwin();
    refresh();
    // Resize and reposition subwindows
    //wresize(input_win, 3, COLS);             // search bar
    wresize(results_win, LINES - 3, COLS);       // results window
    //mvwin(input_win, 0, 0);
    mvwin(results_win, LINES - 3, 0);          // Move results

    // Redraw input window
    werase(input_win);
    box(input_win, 0, 0);
    // mvwprintw(input_win, 1, 1, "Status: %d rows, %d cols", 4, COLS);
    wnoutrefresh(input_win);

    // Redraw results window
    werase(results_win);
    box(results_win, 0, 0);
    // mvwprintw(results_win, 1, 1, "Main Window (%d x %d)", LINES - 4, COLS);
    wnoutrefresh(results_win);

    doupdate();
}

size_t _arr_length(char **arr) {
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

#ifdef _WIN32
    const int FIXED_ROWS = 44;
    const int FIXED_COLS = 82;
    _set_fixed_console_size(FIXED_ROWS, FIXED_COLS);
#endif

    // Get screen dimensions
    // int max_y, max_x;
    // getmaxyx(stdscr, max_y, max_x);

    // Create input and results windows and bg win for results
    input_win = newwin(4, COLS, 0, 0);
    box(input_win, 0, 0);
    mvwprintw(input_win, 1, 1, "Search: ");
    wrefresh(input_win);
    keypad(input_win, TRUE);

    bg_win = newwin(LINES - 4, COLS, 4, 0);
    box(bg_win, 0, 0);
    wrefresh(bg_win);

    results_win = newwin(LINES - 6, COLS - 2, 5, 1);
    wrefresh(results_win);
    keypad(results_win, TRUE);


    //get height of results window to know how many results to display
    int results_max_y = getmaxy(results_win);

    // set num of results to display based on size of results window
    int num_results_to_display = results_max_y - 2;

    // Buffers and variables for user interaction
    char input[MAX_INPUT] = "";
    // char **block_lines;
    // char *content_copy;
    // int num_lines = 0;
    char *content_copy2;
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
        // setup input win
        box(input_win, 0, 0);
        mvwprintw(input_win, 1, 1, "Search: ");
        wrefresh(input_win);




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

        // sort matches and match_scores simultaneously by score using bubble sort
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
        int ch = wgetch(input_win);

        if (ch == 27) {
            break;
        } else if (ch == '\n' && num_matches > 0) {
            werase(results_win);

            // original line
            mvwprintw(results_win, 0, 0, "Credentials for %s:\n\n%s", 
                creds[matches[selected]].account, creds[matches[selected]].content);
            
            mvwprintw(results_win, LINES - 8, 1, "Press 'e' to edit");
            mvwprintw(results_win, LINES - 7, 1, "Press any other key to continue...");
            wrefresh(results_win);
            
            // wait for key press
            int ch = wgetch(results_win);
            if (ch == 'e') {
                // copy content buffer, gets new address for copy
                content_copy2 = strdup(creds[matches[selected]].content);
                //realloc to max size before editing
                content_copy2 = realloc(content_copy2, MAX_BLOCK_SIZE);

                // open edit window
                edit_cred(content_copy2, creds[matches[selected]].content,
                    creds[matches[selected]].account, creds, num_creds);
                
                erase();
                refresh();
                box(bg_win, 0, 0);
                wrefresh(bg_win);
                wrefresh(results_win);
                
                free(content_copy2);
            }
            

        // key codes needed to be modified for wine - KEY_UP, etc. did not work
        // 450 = KEY_UP, 456 = KEY_DOWN, 8 = KEY_BACKSPACE
        } else if (ch == KEY_UP && selected > 0) {
            selected--;
        } else if (ch == KEY_DOWN && selected < num_matches - 1) {
            selected++;
        } else if (ch == KEY_BACKSPACE || ch == 8) {
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
        } else if (ch == KEY_RESIZE) {
            _redraw_ui(); // Handle resize immediately

            //get new height of results window to know how many results to display
            results_max_y = getmaxy(results_win);

            // set new num of results to display based on new size of results window
            num_results_to_display = results_max_y - 2;
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