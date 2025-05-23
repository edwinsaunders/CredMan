#include "cm-main.h"

// Function to get the length of a logical line starting at a given position
int _get_line_length(const char *text, int start_pos, int text_len) {
    int len = 0;
    while (start_pos + len < text_len && text[start_pos + len] != '\n') {
        len++;
    }
    return len;
}

// Function to calculate cursor (x, y) from buffer position, accounting for wrapping
void _calc_cursor_pos(const char *buffer, int buffer_pos, int win_width, int *cursor_x, int *cursor_y) {
    *cursor_x = 0;
    *cursor_y = 0;
    for (int i = 0; i < buffer_pos; i++) {
        if (buffer[i] == '\n') {
            *cursor_y += 1;
            *cursor_x = 0;
        } else if (*cursor_x >= win_width - 1) {
            *cursor_y += 1;
            *cursor_x = 0;
        } else {
            *cursor_x += 1;
        }
        
    }
}

// Function to calculate buffer position from cursor (x, y), accounting for wrapping
int _calc_buffer_pos(const char *buffer, int cursor_x, int cursor_y, int win_width, int text_len) {
    int buf_pos = 0, x = 0, y = 0;
    for (int i = 0; i < text_len && y < cursor_y; i++) {
        buf_pos++;
        if (buffer[i] == '\n') {
            y++;
            x = 0;
        } else if (x >= win_width) {
            y++;
            x = 0;
        } else {
            x++;
        }
    }
    // Move to correct x position in target line
    while (x < cursor_x && buf_pos < text_len && buffer[buf_pos] != '\n' && x < win_width) {
        buf_pos++;
        x++;
    }
    return buf_pos > text_len ? text_len : buf_pos;
}

// Function to render text with wrapping
void _render_text(WINDOW *win, const char *buffer, int text_len, int win_width, int win_height) {
    wclear(win);
    int x = 0, y = 0;
    for (int i = 0; i < text_len && y < win_height; i++) {
        if (buffer[i] == '\n') {
            y++;
            x = 0;
        } else {
            if (x >= win_width) {
                y++;
                x = 0;
            }
            if (y < win_height) {
                mvwaddch(win, y, x, buffer[i]);
            }
            x++;
        }
    }
}

void edit_cred(char *tempbuffer, char *savebuffercontent, char *savebufferaccount, Credential *creds, int num_creds) {
    // Create windows
    int win_height = LINES * 0.8;
    int win_width = COLS * 0.8;
    int start_y = (LINES - win_height) / 2;
    int start_x = (COLS - win_width) / 2;

    WINDOW *edit_win = newwin(win_height - 4, win_width, start_y, start_x);
    WINDOW *bg_win = newwin(win_height + 2, win_width + 2, start_y - 1, start_x - 1);
    if (!edit_win || !bg_win) {
        return;
    }

    //display control keys
    int bg_max_y = getmaxy(bg_win);
    mvwprintw(bg_win, bg_max_y - 2, 1, "Esc to return without saving, CTRL+S to save and return");

    // Initialize ncurses settings
    keypad(edit_win, TRUE);
    box(bg_win, 0, 0);
    wrefresh(bg_win);

    // Initialize text and cursor
    int text_len = strlen(tempbuffer);
    int buffer_pos = text_len;
    int cursor_x, cursor_y;
    _calc_cursor_pos(tempbuffer, buffer_pos, win_width, &cursor_x, &cursor_y);

    // Render initial text
    _render_text(edit_win, tempbuffer, text_len, win_width, win_height);
    wmove(edit_win, cursor_y, cursor_x);
    wrefresh(edit_win);

    // Text editing loop
    int ch;
    while ((ch = wgetch(edit_win)) != 27) { // Exit on ESC
        if (ch == '\n') { // Handle Enter
            if (text_len < MAX_BLOCK_SIZE - 1) {
                memmove(&tempbuffer[buffer_pos + 1], &tempbuffer[buffer_pos], text_len - buffer_pos + 1);
                tempbuffer[buffer_pos] = '\n';
                text_len++;
                buffer_pos++;
                _calc_cursor_pos(tempbuffer, buffer_pos, win_width, &cursor_x, &cursor_y);
                _render_text(edit_win, tempbuffer, text_len, win_width, win_height);
            }
        } else if (ch == KEY_BACKSPACE || ch == 8) { // Handle Backspace
            if (buffer_pos > 0) {
                memmove(&tempbuffer[buffer_pos - 1], &tempbuffer[buffer_pos], text_len - buffer_pos + 1);
                text_len--;
                buffer_pos--;
                _calc_cursor_pos(tempbuffer, buffer_pos, win_width, &cursor_x, &cursor_y);
                _render_text(edit_win, tempbuffer, text_len, win_width, win_height);
            }
        } else if (ch == KEY_DC) { // Handle Delete
            if (buffer_pos < text_len) {
                memmove(&tempbuffer[buffer_pos], &tempbuffer[buffer_pos + 1], text_len - buffer_pos);
                text_len--;
                _calc_cursor_pos(tempbuffer, buffer_pos, win_width, &cursor_x, &cursor_y);
                _render_text(edit_win, tempbuffer, text_len, win_width, win_height);
            }
        } else if (ch >= 32 && ch <= 126) { // Printable characters
            if (text_len < MAX_BLOCK_SIZE - 1) {
                memmove(&tempbuffer[buffer_pos + 1], &tempbuffer[buffer_pos], text_len - buffer_pos + 1);
                tempbuffer[buffer_pos] = ch;
                text_len++;
                buffer_pos++;
                _calc_cursor_pos(tempbuffer, buffer_pos, win_width, &cursor_x, &cursor_y);
                _render_text(edit_win, tempbuffer, text_len, win_width, win_height);
            }
        } else if (ch == KEY_UP) { // Handle Up arrow
            if (cursor_y > 0) {
                cursor_y--;
                buffer_pos = _calc_buffer_pos(tempbuffer, cursor_x, cursor_y, win_width, text_len);
                _calc_cursor_pos(tempbuffer, buffer_pos, win_width, &cursor_x, &cursor_y); // Adjust x if needed
            }
        } else if (ch == KEY_DOWN) { // Handle Down arrow
            int next_y = cursor_y + 1;
            int new_pos = _calc_buffer_pos(tempbuffer, cursor_x, next_y, win_width, text_len);
            int max_y = 0;
            _calc_cursor_pos(tempbuffer, text_len, win_width, &cursor_x, &max_y); // Get max visual lines
            if (next_y <= max_y || (new_pos == text_len && cursor_x <= _get_line_length(tempbuffer, new_pos - 1, text_len))) {
                cursor_y = next_y;
                buffer_pos = new_pos;
                _calc_cursor_pos(tempbuffer, buffer_pos, win_width, &cursor_x, &cursor_y); // Adjust x if needed
                _render_text(edit_win, tempbuffer, text_len, win_width, win_height);
            }
        } else if (ch == KEY_LEFT) { // Handle Left arrow
            if (buffer_pos > 0) {
                buffer_pos--;
                _calc_cursor_pos(tempbuffer, buffer_pos, win_width, &cursor_x, &cursor_y);
                _render_text(edit_win, tempbuffer, text_len, win_width, win_height);
            }
        } else if (ch == KEY_RIGHT) { // Handle Right arrow
            if (buffer_pos < text_len) {
                if (tempbuffer[buffer_pos] == '\n') {
                    continue;
                }
                buffer_pos++;
                _calc_cursor_pos(tempbuffer, buffer_pos, win_width, &cursor_x, &cursor_y);
                _render_text(edit_win, tempbuffer, text_len, win_width, win_height);
            }
        } else if (ch == 19) { // Handle CTRL + S
            memcpy(savebuffercontent, tempbuffer, strlen(tempbuffer) + 1);
            char *account = extract_account(tempbuffer);
            memcpy(savebufferaccount, account, strlen(account) + 1);
            free(account);
            save_credentials(DEFAULT_FILE, creds, num_creds);
            break;
        }

        // Update cursor position
        //mvwprintw(edit_win, edit_max_y - 1, 1, "Esc to return, CTRL+S to save");
        wmove(edit_win, cursor_y, cursor_x);
        wrefresh(edit_win);
    }

    // Clean up
    delwin(edit_win);
    delwin(bg_win);
}