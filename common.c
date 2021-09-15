#include <slurm/slurm.h>
#include <slurm/slurm_errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void print_u32(FILE *file, uint32_t value, bool isEnd) {
  if (value == NO_VAL) {
    fprintf(file, "(null)");
  } else if (value == INFINITE) {
    fprintf(file, "(infinite)");
  } else {
    fprintf(file, "%u", value);
  }
  if (isEnd) {
    fprintf(file, "\n");
  } else {
    fprintf(file, "\t");
  }
}

void print_u64(FILE *file, uint64_t value, bool isEnd) {
  if (value == NO_VAL64) {
    fprintf(file, "(null)");
  } else if (value == INFINITE64) {
    fprintf(file, "(infinite)");
  } else {
    fprintf(file, "%lu", value);
  }
  if (isEnd) {
    fprintf(file, "\n");
  } else {
    fprintf(file, "\t");
  }
}

void print_u16(FILE *file, uint16_t value, bool isEnd) {
  if (value == NO_VAL16) {
    fprintf(file, "(null)");
  } else if (value == INFINITE16) {
    fprintf(file, "(infinite)");
  } else {
    fprintf(file, "%u", value);
  }
  if (isEnd) {
    fprintf(file, "\n");
  } else {
    fprintf(file, "\t");
  }
}

int sprint_time(char *buf, size_t size, time_t time) {
  struct tm *ptm = localtime(&time);
  return strftime(buf, size, "%Y-%m-%d %H:%M:%S", ptm);
}

void print_time(FILE *file, time_t time, bool isEnd) {
  char buf[1024];
  memset(buf, 0, sizeof(buf));
  if (time > 0) {
    struct tm *ptm = localtime(&time);
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", ptm);
    fprintf(file, "%s", buf);
  } else {
    fprintf(file, "(null)");
  }
  if (isEnd) {
    fprintf(file, "\n");
  } else {
    fprintf(file, "\t");
  }
}

int sprint_duration_time(char *buf, size_t size, time_t target_time) {
  time_t current_time = time(NULL);
  time_t diff = current_time - target_time;

  time_t sec = diff % 60;
  diff /= 60;
  time_t min = diff % 60;
  diff /= 60;
  time_t hour = diff % 24;
  diff /= 24;
  time_t day = diff;

  if (day == 0) {
    return snprintf(buf, size, "%02ld:%02ld:%02ld", hour, min, sec);
  } else {
    return snprintf(buf, size, "%ldd %02ld:%02ld:%02ld", day, hour, min, sec);
  }
}

int sprint_diff_time(char *buf, size_t size, time_t diff) {
  time_t sec = diff % 60;
  diff /= 60;
  time_t min = diff % 60;
  diff /= 60;
  time_t hour = diff % 24;
  diff /= 24;
  time_t day = diff;

  if (day == 0) {
    return snprintf(buf, size, "%02ld:%02ld:%02ld", hour, min, sec);
  } else {
    return snprintf(buf, size, "%ldd %02ld:%02ld:%02ld", day, hour, min, sec);
  }
}

long string2long(const char *str, bool *ok) {
  char *endptr = NULL;
  long value = strtol(str, &endptr, 10);
  if (*str != '\0' && *endptr == '\0') {
    *ok = true;
    return value;
  }
  *ok = false;
  return 0;
}

long long string2longlong(const char *str, bool *ok) {
  char *endptr = NULL;
  long long value = strtoll(str, &endptr, 10);
  if (*str != '\0' && *endptr == '\0') {
    *ok = true;
    return value;
  }
  *ok = false;
  return 0;
}

double string2double(const char *str, bool *ok) {
  char *endptr = NULL;
  long long value = strtod(str, &endptr);
  if (*str != '\0' && *endptr == '\0') {
    *ok = true;
    return value;
  }
  *ok = false;
  return 0;
}

int giga_memory(char *buf, size_t size, uint64_t mem) {
  if (mem == NO_VAL64) {
    return snprintf(buf, size, "(null)");
  }
  double double_mem = mem;
  return snprintf(buf, size, "%.1f", double_mem / 1024);
}

int job_id(char *buf, size_t size, slurm_job_info_t *job) {
  if (job->array_task_str != NULL) {
    return snprintf(buf, size, "%u_%s", job->array_job_id, job->array_task_str);
  } else if (job->array_task_id == NO_VAL) {
    return snprintf(buf, size, "%u", job->job_id);
  } else {
    return snprintf(buf, size, "%u_%u", job->array_job_id, job->array_task_id);
  }
}
