#include "term.h"
#include "spawn.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

void term_set_foreground_color(FILE *file, color_t color) {
  if (!isatty(STDOUT_FILENO)) {
    return;
  }
  int color_code;
  switch (color) {
  case TERM_BLACK:
    color_code = 30;
    break;
  case TERM_RED:
    color_code = 31;
    break;
  case TERM_GREEN:
    color_code = 32;
    break;
  case TERM_YELLOW:
    color_code = 33;
    break;
  case TERM_BLUE:
    color_code = 34;
    break;
  case TERM_MAGENTA:
    color_code = 35;
    break;
  case TERM_CYAN:
    color_code = 36;
    break;
  case TERM_WHITE:
    color_code = 37;
    break;
  case TERM_DEFAULT:
  default:
    color_code = 39;
    break;
  }

  fprintf(file, "\x1b[%dm", color_code);
}

void term_set_background_color(FILE *file, color_t color) {
  if (!isatty(STDOUT_FILENO)) {
    return;
  }
  int color_code;
  switch (color) {
  case TERM_BLACK:
    color_code = 40;
    break;
  case TERM_RED:
    color_code = 41;
    break;
  case TERM_GREEN:
    color_code = 42;
    break;
  case TERM_YELLOW:
    color_code = 43;
    break;
  case TERM_BLUE:
    color_code = 44;
    break;
  case TERM_MAGENTA:
    color_code = 45;
    break;
  case TERM_CYAN:
    color_code = 46;
    break;
  case TERM_WHITE:
    color_code = 47;
    break;
  case TERM_DEFAULT:
  default:
    color_code = 49;
    break;
  }

  fprintf(file, "\x1b[%dm", color_code);
}

void term_reset(FILE *file) {
  if (!isatty(STDOUT_FILENO)) {
    return;
  }
  fprintf(file, "\x1b[0m");
}

void term_set_bold(FILE *file, bool enable) {
  if (!isatty(STDOUT_FILENO)) {
    return;
  }
  if (enable) {
    fprintf(file, "\x1b[1m");
  } else {
    fprintf(file, "\x1b[22m");
  }
}

void term_set_underline(FILE *file, bool enable) {
  if (!isatty(STDOUT_FILENO)) {
    return;
  }
  if (enable) {
    fprintf(file, "\x1b[4m");
  } else {
    fprintf(file, "\x1b[24m");
  }
}

void term_set_reverse(FILE *file, bool enable) {
  if (!isatty(STDOUT_FILENO)) {
    return;
  }
  if (enable) {
    fprintf(file, "\x1b[7m");
  } else {
    fprintf(file, "\x1b[27m");
  }
}

pager_t auto_less() {
  if (isatty(STDOUT_FILENO)) {
    char *args[] = {"less", "-R", "-F", NULL};
    child_process_t process =
        spawn_pipe("/bin/less", args, SPAWN_PIPE_STDIN, false);
    if (process.pid < 0) {
      perror("Failed to start less");
      exit(1);
    }
    // fprintf(stderr, "Spawn: %d %d\n", process.pid, process.fd_stdin);
    pager_t pager;
    memset(&pager, 0, sizeof(pager));
    pager.out = fdopen(process.fd_stdin, "w");
    if (pager.out == NULL) {
      perror("Failed to open pager");
      exit(1);
    }
    // pager.out = stdout;
    pager.process = process;
    return pager;
  } else {
    pager_t pager;
    memset(&pager, 0, sizeof(pager));
    pager.out = stdout;
    return pager;
  }
}

void close_pager(pager_t *pager) {
  if (pager->process.pid > 0) {
    int status;
    fclose(pager->out);
    waitpid(pager->process.pid, &status, 0);
  }
}
