#include <stdio.h>
#include <stdlib.h>
#include <curses.h>

#define CURRENT_CHAR   1
#define PREVIOUS_CHARS 2
#define NEXT_CHARS     3

int main (int argc, char *argv[]) {
    int max_y, max_x, win_y, win_x, y, x;
    char ch;

    WINDOW *win;

    /* curses スクリーンの初期化 */

    initscr ();
    curs_set (0);
    noecho ();
    cbreak ();

    /* 色の準備 */

    if (!has_colors ()) {
        endwin ();
        printf ("colors are not supported.");
        return 0;
    }

    start_color ();

    init_pair (CURRENT_CHAR,   COLOR_BLACK,  COLOR_YELLOW);
    init_pair (PREVIOUS_CHARS, COLOR_YELLOW, COLOR_BLACK);
    init_pair (NEXT_CHARS,     COLOR_WHITE,  COLOR_BLACK);

    /* TODO: スタート画面的な */

    /* 使う変数の初期化 */

    getmaxyx (stdscr, max_y, max_x);

    /* 画面サイズ小さかったら終了 */

    if (!(max_y > 23 && max_x > 79)) {
        endwin ();
        printf ("window size should be greater than or equal to 80 x 24.\n");
        return 0;
    }

    win_y = max_y - 4;
    win_x = max_x - 28;

    /* 四角いウィンドウ作る */
    win = subwin (stdscr, win_y, win_x, 2, 14);
    /* 外枠 */
    box (win, 0, 0);

    y = 1;
    x = 1;

    mvwaddstr (win, y, x, "test");
    mvwaddstr (win, y + 2, x, "test");

    getch ();

    delwin (win);
    endwin ();

    return 0;
}
