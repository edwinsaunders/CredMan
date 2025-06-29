#include "cm-main.h"




// get number of visual lines
int _get_vis_lines(const char *buffer, int win_width) {
    if (buffer[0] == '\0') {
        return 1;
    }

    // the answer is the number of '\n' + 1 + the number of times we hit the win_width


    int x = 0;
    int num_newlines = 0;
    int num_word_wraps = 0;
    int text_len = strlen(buffer);
    for (int i = 0; i < text_len; i++) {
        if (buffer[i] == '\n') {
            num_newlines++;
            x = 0;
        } else if (x == win_width) {
            num_word_wraps++;
            x = 0;
        } else {
            x++;
        }
    }
    //int x = 1;
    //int y = 1;
    //for (int i = 0; i < strlen(buffer); i++) {
    //    if (buffer[i] == '\n') {
    //        y++;
    //        x = 1;
    //    } else if (x >= win_width) {
    //        y++;
    //        //printf("buffer at i: %c\nx: %d\ni: %d\nwin_width: %d\n", buffer[i], x, i, win_width);
    //        x = 1;
    //    } else {
    //        x++;
    //    }
    //}
    
    // There is one more line than there are newline chars
    return num_word_wraps + num_newlines + 1;
}

// Function to get the length of a logical line starting at a given position
//int _get_line_length_from(const char *text, int start_pos, int text_len) {
//    int len = 0;
//    while (start_pos + len < text_len && text[start_pos + len] != '\n') {
//        len++;
//    }
//    return len;
//}

// Function to get the length of a visual line, taking the pos of first char after a new visual line as start
int _get_line_length(const char *text, int start_pos, int text_len, int win_width) {
    int len = 0;
    
    if (start_pos >= text_len) return 0;

    //decrement buf_pos to char after last newline
    //while (text[start_pos] != '\n' && start_pos > 0) start_pos--;
    //start_pos++;

    //count chars up to '\n' or win_width
    while (text[len + start_pos] != '\n' && len < win_width) len++;
    

    return len;
}

//function to get line length for up/down cursor movement x positioning
int _get_target_line_length(const char *buffer, int cursor_y, int win_width, int text_len) {
    int y = 0;
    int x = 0;
    int length = 0;
    
    int i = 0;
    while (y < cursor_y && i < text_len) {
        if (buffer[i] == '\n') {
            y++;
            x = 0;
        } else if (x == win_width - 1) {
            y++;
            x = 0;
        } else {
            x++; 
        }
        i++;                
    }

    int num_to_subtract = i;
    while (buffer[i] != '\n' && i < text_len) {
        i++;
    }
    length = i - num_to_subtract;

    //printf("%c\n", buffer[i]);

    return length;
}
    
 


// update cursor (x, y) from buffer position, accounting for wrapping
void _update_cursor_pos(const char *buffer, int buffer_pos, int win_width, int *cursor_x, int *cursor_y) {
    *cursor_x = 0;
    *cursor_y = 0;
    for (int i = 0; i < buffer_pos; i++) {
        if (buffer[i] == '\n') {
            *cursor_y += 1;
            *cursor_x = 0;
        } else if (*cursor_x == win_width - 1) {
            *cursor_y += 1;
            *cursor_x = 0;
        } else {
            //if (i < strlen(buffer)) *cursor_x += 1;
            *cursor_x += 1;
        }
    }
    //if (*cursor_x == win_width) {
    //    *cursor_x -= 1;
    //}
}

// Function to calculate buffer position from cursor (x, y), accounting for wrapping
int _calc_buffer_pos(const char *buffer, int cursor_x, int cursor_y, int win_width, int text_len) {
    int buf_pos = 0;
    int x = 0;
    int y = 0;
    int line_start = 0;
    int line_len = 0;                     //change

    // use y to get to the first char on target row
    // while (y < cursor_y) {
    //     i++;
    //     if (buffer[i] == '\n') {
    //         y++;
    //         x = 0;
    //     }
// 
    // }
    for (int i = 0; i < text_len; i++) {    //change
        if (buffer[i] == '\n') {
            y++;
            line_start = i + 1;
            x = 0;             //change; deletion of x = 0;
        } else if (x >= win_width) {
            y++;
            line_start = i;                 //change
            x = 0;
        } else {
            x++;
        }
        if (y == cursor_y) {                //addition
            buf_pos = line_start;           //addition
            break;                          //addition
        }                                   //addition
    }
    //double-check that cursor is not somehow on a row past the end of the text
    if (y < cursor_y) {
        buf_pos = text_len;
    } else {
        // Adjust for cursor_x on the target line
        // line_len is num chars without '\n'
        line_len = _get_line_length(buffer, buf_pos, text_len, win_width);
        // cursor_x + 2 shuld be 1 past the last char, since + 1 is the actual num chars
        if (line_len <= cursor_x) {
            buf_pos += line_len; // Move to end of line
        } else {
            buf_pos += cursor_x; // Move to desired x position
        }
    }

    return buf_pos;
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
    // int win_width = 20;
    int start_y = (LINES - win_height) / 2;
    int start_x = (COLS - win_width) / 2;
    
    // number of visual lines as opposed to logical '\n' lines
    int num_vis_lines = 0;
    // goal_column, may not be necessary since it always equals cursor_x
    int goal_col = 0;
    //length of line
    int line_len = 0;
    //start position of a line
    //int start_pos = 0;


    WINDOW *edit_win = newwin(win_height - 4, win_width, start_y, start_x);
    WINDOW *bg_win = newwin(win_height + 2, win_width + 2, start_y - 1, start_x - 1);
    if (!edit_win || !bg_win) {
        return;
    }

    //display control keys
    int bg_max_y = getmaxy(bg_win);
    //int edit_max_y = getmaxy(edit_win);
    mvwprintw(bg_win, bg_max_y - 2, 1, "Esc to return without saving, CTRL+S to save and return");

    // Initialize ncurses settings
    keypad(edit_win, TRUE);
    box(bg_win, 0, 0);
    wrefresh(bg_win);

    // Initialize text and cursor
    int text_len = strlen(tempbuffer);
    int buffer_pos = text_len;
    int cursor_x, cursor_y;
    _update_cursor_pos(tempbuffer, buffer_pos, win_width, &cursor_x, &cursor_y);
    goal_col = cursor_x;

    // Render initial text
    _render_text(edit_win, tempbuffer, text_len, win_width, win_height);
    wmove(edit_win, cursor_y, cursor_x);
    wrefresh(edit_win);

    // Text editing loop
    int ch;
    while ((ch = wgetch(edit_win)) != 27) { // Exit on ESC
        if (ch == '\n') { // Handle Enter
            // check if we've reached the max block size
            if (text_len < MAX_BLOCK_SIZE) {
                memmove(&tempbuffer[buffer_pos + 1], &tempbuffer[buffer_pos], text_len - buffer_pos + 1);
                tempbuffer[buffer_pos] = '\n';
                text_len++;
                buffer_pos++;
                _update_cursor_pos(tempbuffer, buffer_pos, win_width, &cursor_x, &cursor_y);
                goal_col = cursor_x;
                _render_text(edit_win, tempbuffer, text_len, win_width, win_height);
            }
        } else if (ch == KEY_BACKSPACE || ch == 8) { // Handle Backspace
            if (buffer_pos > 0) {
                memmove(&tempbuffer[buffer_pos - 1], &tempbuffer[buffer_pos], text_len - buffer_pos + 1);
                text_len--;
                buffer_pos--;
                _update_cursor_pos(tempbuffer, buffer_pos, win_width, &cursor_x, &cursor_y);
                goal_col = cursor_x;
                _render_text(edit_win, tempbuffer, text_len, win_width, win_height);
            }
        } else if (ch == KEY_DC) { // Handle Delete
            if (buffer_pos < text_len) {
                memmove(&tempbuffer[buffer_pos], &tempbuffer[buffer_pos + 1], text_len - buffer_pos + 1);
                text_len--;
                _update_cursor_pos(tempbuffer, buffer_pos, win_width, &cursor_x, &cursor_y);
                _render_text(edit_win, tempbuffer, text_len, win_width, win_height);
            }
        } else if (ch >= 32 && ch <= 126) { // Printable characters
            if (text_len < MAX_BLOCK_SIZE) {
                memmove(&tempbuffer[buffer_pos + 1], &tempbuffer[buffer_pos], text_len - buffer_pos + 1);
                tempbuffer[buffer_pos] = ch;
                text_len++;
                buffer_pos++;
                _update_cursor_pos(tempbuffer, buffer_pos, win_width, &cursor_x, &cursor_y);
                goal_col = cursor_x;
                _render_text(edit_win, tempbuffer, text_len, win_width, win_height);
            }
        } else if (ch == KEY_UP) { // Handle Up arrow
            if (cursor_y > 0) {
                cursor_y--;
                // get new cursor_x
                // target line length (will equal index of '\n' on target line)
                line_len = _get_target_line_length(tempbuffer, cursor_y, win_width, text_len);
                    //get length of target line and compare with current goal col
                    if (line_len < goal_col) {
                        cursor_x = line_len;
                    } else {
                        cursor_x = goal_col;
                    }
                buffer_pos = _calc_buffer_pos(tempbuffer, cursor_x, cursor_y, win_width, text_len);
                //_update_cursor_pos(tempbuffer, buffer_pos, win_width, &cursor_x, &cursor_y); // Adjust x if needed
            }
            //debug line to print number of lines on screen
            // mvwprintw(edit_win, edit_max_y - 7, 1, "num_vis_lines: %d", num_vis_lines);
            // mvwprintw(edit_win, edit_max_y - 6, 1, "cursor_x: %d, cursor_y: %d, goal_col: %d", cursor_x, cursor_y, goal_col);
            // mvwprintw(edit_win, edit_max_y - 5, 1, "buffer_pos: %d\n", buffer_pos);
            // mvwprintw(edit_win, edit_max_y - 4, 1, "buffer: %s\n", tempbuffer);
            // wmove(edit_win, cursor_y, cursor_x);
        } else if (ch == KEY_DOWN) { // Handle Down arrow
            // down arrow needs to know how may lines there are and figure out 
            // if cursor can go there based on current y pos and num visual lines
            num_vis_lines = _get_vis_lines(tempbuffer, win_width);

            // if we're not on the last line
            if (cursor_y < num_vis_lines - 1) {
                cursor_y++;
                // get new cursor_x
                // target line length (will equal index of '\n' on target line)
                line_len = _get_target_line_length(tempbuffer, cursor_y, win_width, text_len);
                    //get length of target line and compare with current goal col
                    if (line_len < goal_col) {
                        cursor_x = line_len;
                    } else {
                        cursor_x = goal_col;
                    }

                buffer_pos = _calc_buffer_pos(tempbuffer, cursor_x, cursor_y, win_width, text_len);
                //_update_cursor_pos(tempbuffer, buffer_pos, win_width, &cursor_x, &cursor_y); // Adjust x if needed
            }
            //debug line to print number of lines on screen
            // mvwprintw(edit_win, edit_max_y - 7, 1, "num_vis_lines: %d", num_vis_lines);
            // mvwprintw(edit_win, edit_max_y - 6, 1, "cursor_x: %d, cursor_y: %d, goal_col: %d", cursor_x, cursor_y, goal_col);
            // mvwprintw(edit_win, edit_max_y - 5, 1, "buffer_pos: %d\n", buffer_pos);
            // mvwprintw(edit_win, edit_max_y - 4, 1, "buffer: %s\n", tempbuffer);
            // wmove(edit_win, cursor_y, cursor_x);

        } else if (ch == KEY_LEFT) { // Handle Left arrow
            if (buffer_pos > 0) {
                buffer_pos--;
                _update_cursor_pos(tempbuffer, buffer_pos, win_width, &cursor_x, &cursor_y);
                goal_col = cursor_x;
                //_render_text(edit_win, tempbuffer, text_len, win_width, win_height);
            }
        } else if (ch == KEY_RIGHT) { // Handle Right arrow
            if (buffer_pos < text_len) {
                //if (tempbuffer[buffer_pos] == '\n') {
                //    continue;
                //}
                buffer_pos++;
                _update_cursor_pos(tempbuffer, buffer_pos, win_width, &cursor_x, &cursor_y);
                goal_col = cursor_x;
                //_render_text(edit_win, tempbuffer, text_len, win_width, win_height);
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
        wmove(edit_win, cursor_y, cursor_x);
        wrefresh(edit_win);
    }

    // Clean up
    delwin(edit_win);
    delwin(bg_win);
}
