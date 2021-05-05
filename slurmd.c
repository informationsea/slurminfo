#include "slurmd.h"
#include "common.h"
#include <slurm/slurm.h>
#include <slurm/slurm_errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void print_slurmd(FILE *file) {
  slurmd_status_t *info;

  if (slurm_load_slurmd_status(&info)) {
    slurm_perror("slurm_load_slurmd_status error");
    exit(1);
  }

  slurm_print_slurmd_status(file, info);

  slurm_free_slurmd_status(info);
}
