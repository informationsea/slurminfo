#ifndef _COMMON_H
#define _COMMON_H

#include <slurm/slurm.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) < (b) ? (b) : (a))

#define STR(x) _STR(x)
#define _STR(x) #x

#define LOGF(type, message, ...)                                               \
  fprintf(stderr, type "(" __FILE__ "[%3u]: %s): " message "\n", __LINE__,     \
          __func__, __VA_ARGS__)
#define LOG(type, message)                                                     \
  fprintf(stderr, type "(" __FILE__ "[%3u]: %s): %s\n", __LINE__, __func__,    \
          message)

#define INFOF(...) LOGF("INFO ", __VA_ARGS__)
#define INFO(message) LOG("INFO ", message)

#define WARNF(...) LOGF("WARN ", __VA_ARGS__)
#define WARN(message) LOG("WARN ", message)

#define ERRORF(...) LOGF("ERROR", __VA_ARGS__)
#define ERROR(message) LOG("ERROR", message)

#define LENGTH(x) (sizeof(x) / sizeof((x)[0]))

void print_u32(FILE *file, uint32_t value, bool isEnd);
void print_u64(FILE *file, uint64_t value, bool isEnd);
void print_u16(FILE *file, uint16_t value, bool isEnd);
void print_time(FILE *file, time_t time, bool isEnd);
int sprint_time(char *buf, size_t size, time_t time);
int sprint_duration_time(char *buf, size_t size, time_t target_time);
int sprint_diff_time(char *buf, size_t size, time_t diff);
long string2long(const char *str, bool *ok);
long long string2longlong(const char *str, bool *ok);
double string2double(const char *str, bool *ok);
int giga_memory(char *buf, size_t size, uint64_t mem);
int job_id(char *buf, size_t size, slurm_job_info_t *job);

#ifdef __cplusplus
}
#endif

#endif /* _COMMON_H */
