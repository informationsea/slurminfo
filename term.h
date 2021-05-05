#ifndef _TERM_H
#define _TERM_H

#include "spawn.h"
#include <stdbool.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  TERM_BLACK,
  TERM_RED,
  TERM_GREEN,
  TERM_YELLOW,
  TERM_BLUE,
  TERM_MAGENTA,
  TERM_CYAN,
  TERM_WHITE,
  TERM_DEFAULT
} color_t;

void term_set_foreground_color(FILE *file, color_t color);
void term_set_background_color(FILE *file, color_t color);
void term_reset(FILE *file);
void term_set_bold(FILE *file, bool enable);
void term_set_underline(FILE *file, bool enable);
void term_set_reverse(FILE *file, bool enable);

typedef struct {
  FILE *out;
  child_process_t process;
} pager_t;

pager_t auto_less();
void close_pager(pager_t *pager);

#ifdef __cplusplus
}
#endif

#endif /* _TERM_H */
