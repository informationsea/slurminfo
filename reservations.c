#include "common.h"
#include <slurm/slurm.h>
#include <slurm/slurm_errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void print_reservations(FILE *file) {
  reserve_info_msg_t *res_info_ptr = NULL;

  /* get and dump all reservation information */
  if (slurm_load_reservations((time_t)NULL, &res_info_ptr)) {
    slurm_perror("slurm_load_reservations error");
    exit(1);
  }

  /* The easy way to print... */
  slurm_print_reservation_info_msg(file, res_info_ptr, 0);
}
