#include <assert.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define MAX_CHARS     500
#define MAX_SENTENCES 1000
#define MAX_MISSCOUNT 1000

#define CURRENT_CHAR   1
#define PREVIOUS_CHARS 2
#define NEXT_CHARS     3

int  read_sentences_from_file (char *, char **);
void draw_window (WINDOW *, char *, char *, char *, int, int);

int main (int argc, char *argv[]) {
  char * p, *curr, *head, *tail;
  char   ch;
  int    misscount, correctcount;
  int    max_y, max_x, win_y, win_x, center_y, center_x, y, x;
  time_t s_time, e_time;

  WINDOW *win;

  char *sentences[MAX_SENTENCES];
  char *filename = argv[1];

  int sentences_amount = read_sentences_from_file (filename, sentences);
  if (sentences_amount == -1) {
    printf ("cannot open file %s.\n", filename);
    return 0;
  }

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

  init_pair (CURRENT_CHAR, COLOR_BLACK, COLOR_YELLOW);
  init_pair (PREVIOUS_CHARS, COLOR_YELLOW, COLOR_BLACK);
  init_pair (NEXT_CHARS, COLOR_WHITE, COLOR_BLACK);

  /* TODO: スタート画面的な */

  /* 使う変数の初期化 */

  correctcount = 0;
  misscount    = 0;

  getmaxyx (stdscr, max_y, max_x);

  /* 画面サイズ小さかったら終了 */

  if (!(max_y > 23 && max_x > 79)) {
    endwin ();
    printf ("window size should be greater than or equal to 80 x 24.\n");
    return 0;
  }

  win_y = max_y - 4;
  win_x = max_x - 28;

  center_y = win_y / 2;
  center_x = (win_x + 1) / 2 - 1;

  /* 四角いウィンドウ作る */
  win = subwin (stdscr, win_y, win_x, 2, 14);

  /* 外枠 */
  box (win, 0, 0);

  /* 以下メイン処理 */

  /* 開始時間 */
  time (&s_time);

  for (int i = 0; i < sentences_amount; i++) {
    curr = sentences[i];
    head = sentences[i];
    tail = sentences[i];
    while (tail[1] != '\0') tail++;

    if (*curr == '\0') break;

    while (*curr != '\0') {
      // 描画
      draw_window (win, curr, head, tail, center_x, center_y);

      // キー入力
      ch = getch ();
      assert (ch != -1);

      // 正解ならば次の文字に進む
      if (ch == *curr) {
        correctcount++;
        curr++;
      }

      else {
        misscount++;

        /* ミスしすぎはハゲ */
        if (misscount > MAX_MISSCOUNT) {
          endwin ();
          printf ("too much mistakes!\n");
          return 0;
        }

        mvwaddch (win, center_y - 1, center_x, ch);
      }

      box (win, 0, 0);
      wrefresh (win);
    }

    wclear (win);
    /* 外枠 */
    box (win, 0, 0);
    touchwin (win);
  }

  /* 終了時間 */
  time (&e_time);

  /* TODO: リザルト的な */
  /* TODO: メニュー的な */

  delwin (win);
  endwin ();

  for (int i = 0; i < sentences_amount; i++) free (sentences[i]);

  printf ("time:    %ld\n", e_time - s_time);
  printf ("total:   %d\n", correctcount + misscount);
  printf ("correct: %d\n", correctcount);
  printf ("miss:    %d\n", misscount);

  return 0;
}

/**
 * 外部ファイルから使う文を読み込み
 * 成功時は行数 失敗時は -1 を返す
 */
int read_sentences_from_file (char *filename, char **sentences) {
  int   i = 0;
  FILE *fp;
  char  str[MAX_CHARS + 1];

  fp = fopen (filename, "r");

  if (fp == NULL) return -1;

  while (fgets (str, MAX_CHARS, fp) != NULL && i <= MAX_SENTENCES) {
    /* 雑だけど最後の改行文字を消す */
    if (str[strlen (str) - 1] == '\n') str[strlen (str) - 1] = '\0';

    if (strlen (str) != 0) {
      sentences[i] = (char *) malloc (sizeof (char) * strlen (str) + 1);
      strcpy (sentences[i++], str);
    }
  }

  fclose (fp);
  return i;
}

void draw_window (WINDOW *win, char *curr, char *head, char *tail, int center_x, int center_y) {
  int   x, y;
  char *p;
  int   max_y, max_x;

  getmaxyx (stdscr, max_y, max_x);

  wclrtoeol (win);

  /* 前の部分の描画 */
  p = curr;

  y = center_y;
  x = center_x;

  wattron (win, COLOR_PAIR (PREVIOUS_CHARS));
  while (--p >= head && --x >= 1) mvwaddch (win, y, x, *p);
  wattroff (win, COLOR_PAIR (PREVIOUS_CHARS));

  /* 後の部分の描画 */
  p = curr;

  y = center_y;
  x = center_x;

  wattron (win, COLOR_PAIR (NEXT_CHARS));
  while (++p <= tail && ++x <= max_x - 2) mvwaddch (win, y, x, *p);
  wattroff (win, COLOR_PAIR (NEXT_CHARS));

  /* 現在の位置 */
  p = curr;

  y = center_y;
  x = center_x;

  wattron (win, COLOR_PAIR (CURRENT_CHAR));
  mvwaddch (win, y, x, *curr);
  wattroff (win, COLOR_PAIR (CURRENT_CHAR));

  box (win, 0, 0);
  touchwin (win);

  wrefresh (win);
}
