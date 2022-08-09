#include "jobs.h"
#include "common.h"
#include <grp.h>
#include <pwd.h>
#include <slurm/slurm.h>
#include <slurm/slurm_errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

void print_job_state(FILE *file, uint32_t job_state) {
  switch (job_state & JOB_STATE_BASE) {
  case JOB_PENDING:
    fprintf(file, "Pending");
    break;
  case JOB_RUNNING:
    fprintf(file, "Running");
    break;
  case JOB_SUSPENDED:
    fprintf(file, "Suspended");
    break;
  case JOB_COMPLETE:
    fprintf(file, "Complete");
    break;
  case JOB_CANCELLED:
    fprintf(file, "Canceled");
    break;
  case JOB_TIMEOUT:
    fprintf(file, "Timeout");
    break;
  case JOB_NODE_FAIL:
    fprintf(file, "NodeFail");
    break;
  case JOB_PREEMPTED:
    fprintf(file, "Preemption");
    break;
  case JOB_BOOT_FAIL:
    fprintf(file, "BootFail");
    break;
  case JOB_DEADLINE:
    fprintf(file, "Deadline");
    break;
  case JOB_OOM:
    fprintf(file, "OOM");
    break;
  default:
    fprintf(file, "Unknown");
    break;
  }
  if (job_state & JOB_LAUNCH_FAILED) {
    fprintf(file, ",LaunchFailed");
  }
  if (job_state & JOB_UPDATE_DB) {
    fprintf(file, ",UpdateDB");
  }
  if (job_state & JOB_REQUEUE) {
    fprintf(file, ",Requeue");
  }
  if (job_state & JOB_REQUEUE_HOLD) {
    fprintf(file, ",RequeueHold");
  }
  if (job_state & JOB_SPECIAL_EXIT) {
    fprintf(file, ",SpecialExit");
  }
  if (job_state & JOB_RESIZING) {
    fprintf(file, ",Resizing");
  }
  if (job_state & JOB_CONFIGURING) {
    fprintf(file, ",Configuring");
  }
  if (job_state & JOB_COMPLETING) {
    fprintf(file, ",Completing");
  }
  if (job_state & JOB_STOPPED) {
    fprintf(file, ",Stopped");
  }
  if (job_state & JOB_RECONFIG_FAIL) {
    fprintf(file, ",JobReconfigFail");
  }
  if (job_state & JOB_POWER_UP_NODE) {
    fprintf(file, ",PowerUpNode");
  }
  if (job_state & JOB_REVOKED) {
    fprintf(file, ",Revoked");
  }
  if (job_state & JOB_REQUEUE_FED) {
    fprintf(file, ",RequeueFed");
  }
  if (job_state & JOB_RESV_DEL_HOLD) {
    fprintf(file, ",Hold");
  }
  if (job_state & JOB_SIGNALING) {
    fprintf(file, ",Signaling");
  }
  if (job_state & JOB_STAGE_OUT) {
    fprintf(file, ",StagingOut");
  }
}

void print_one(FILE *file, const char *text, bool *comma) {
  if (*comma) {
    fprintf(file, ",");
  }
  fprintf(file, "%s", text);
  *comma = true;
}

void print_job_flag(FILE *file, uint32_t flag) {
  bool comma = false;
  if (flag & KILL_INV_DEP) {
    print_one(file, "KillJobOnInvalidDependency", &comma);
  }
  if (flag & NO_KILL_INV_DEP) {
    print_one(file, "NoKillJobOnInvalidDependency", &comma);
  }
  if (flag & HAS_STATE_DIR) {
    print_one(file, "HasStateDir", &comma);
  }
  if (flag & BACKFILL_TEST) {
    print_one(file, "BackfillTestInProgress", &comma);
  }
  if (flag & GRES_ENFORCE_BIND) {
    print_one(file, "EnforceCPU/GRESBinding", &comma);
  }
  if (flag & TEST_NOW_ONLY) {
    print_one(file, "TestImmediatelyStartOnly", &comma);
  }
  if (flag & NODE_REBOOT) {
    print_one(file, "NodeRebootWating", &comma);
  }
  if (flag & SPREAD_JOB) {
    print_one(file, "SpreadJob", &comma);
  }
  if (flag & USE_MIN_NODES) {
    print_one(file, "PreferMinimumNodeCount", &comma);
  }
  if (flag & JOB_KILL_HURRY) {
    print_one(file, "AvoidBurstBufferStageOut", &comma);
  }
  if (flag & TRES_STR_CALC) {
    print_one(file, "AvoidCalculatingTRESString", &comma);
  }
  if (flag & SIB_JOB_FLUSH) {
    print_one(file, "Don'tSendCompleteToOrigin", &comma);
  }
  if (flag & HET_JOB_FLAG) {
    print_one(file, "HeterogeneousJobManagement", &comma);
  }
  if (flag & JOB_NTASKS_SET) {
    print_one(file, "NTaskSet", &comma);
  }
  if (flag & JOB_CPUS_SET) {
    print_one(file, "JobCpusSet", &comma);
  }
  if (flag & BF_WHOLE_NODE_TEST) {
    print_one(file, "BackfillTestInProgress", &comma);
  }
  if (flag & TOP_PRIO_TMP) {
    print_one(file, "TempTopPriorityJob", &comma);
  }
  if (flag & JOB_ACCRUE_OVER) {
    print_one(file, "JobAccrueOver", &comma);
  }
  if (flag & GRES_DISABLE_BIND) {
    print_one(file, "DisableCPUGRESBinding", &comma);
  }
  if (flag & JOB_WAS_RUNNING) {
    print_one(file, "JobWasRunning", &comma);
  }
  if (flag & RESET_ACCRUE_TIME) {
    print_one(file, "ResetAccrueTime", &comma);
  }
  if (flag & JOB_MEM_SET) {
    print_one(file, "JobMemorySet", &comma);
  }
  if (flag & JOB_RESIZED) {
    print_one(file, "JobResized", &comma);
  }
  if (flag & USE_DEFAULT_ACCT) {
    print_one(file, "UseDefaultAccount", &comma);
  }
  if (flag & USE_DEFAULT_PART) {
    print_one(file, "UseDefaultPartition", &comma);
  }
  if (flag & USE_DEFAULT_QOS) {
    print_one(file, "UseDefaultQOS", &comma);
  }
  if (flag & USE_DEFAULT_WCKEY) {
    print_one(file, "UseDefaultWCKEY", &comma);
  }
  if (flag & JOB_DEPENDENT) {
    print_one(file, "JobDependent", &comma);
  }
  if (flag & JOB_MAGNETIC) {
    print_one(file, "JobMagnetic", &comma);
  }
}

const char *get_reason_text(uint16_t state_reason) {
  switch (state_reason) {
  case WAIT_NO_REASON:
    return "(null)";
  case WAIT_PRIORITY:
    return "higher priority jobs exist";
  case WAIT_DEPENDENCY:
    return "dependent job has not completed";
  case WAIT_RESOURCES:
    return "required resources not available";
  case WAIT_PART_NODE_LIMIT:
    return "request exceeds partition node limit";
  case WAIT_PART_TIME_LIMIT:
    return "request exceeds partition time limit";
  case WAIT_PART_DOWN:
    return "requested partition is down";
  case WAIT_PART_INACTIVE:
    return "requested partition is inactive";
  case WAIT_HELD:
    return "job is held by administrator";
  case WAIT_TIME:
    return "job waiting for specific begin time";
  case WAIT_LICENSES:
    return "job is waiting for licenses";
  case WAIT_ASSOC_JOB_LIMIT:
    return "user/bank job limit reached";
  case WAIT_ASSOC_RESOURCE_LIMIT:
    return "user/bank resource limit reached";
  case WAIT_ASSOC_TIME_LIMIT:
    return "user/bank time limit reached";
  case WAIT_RESERVATION:
    return "reservation not available";
  case WAIT_NODE_NOT_AVAIL:
    return "required node is DOWN or DRAINED";
  case WAIT_HELD_USER:
    return "job is held by user";
  case WAIT_FRONT_END:
    return "Front end nodes are DOWN";
  case FAIL_DEFER:
    return "individual submit time sched deferred";
  case FAIL_DOWN_PARTITION:
    return "partition for job is DOWN";
  case FAIL_DOWN_NODE:
    return "some node in the allocation failed";
  case FAIL_BAD_CONSTRAINTS:
    return "constraints can not be satisfied";
  case FAIL_SYSTEM:
    return "slurm system failure";
  case FAIL_LAUNCH:
    return "unable to launch job";
  case FAIL_EXIT_CODE:
    return "exit code was non-zero";
  case FAIL_TIMEOUT:
    return "reached end of time limit";
  case FAIL_INACTIVE_LIMIT:
    return "reached slurm InactiveLimit";
  case FAIL_ACCOUNT:
    return "invalid account";
  case FAIL_QOS:
    return "invalid QOS";
  case WAIT_QOS_THRES:
    return "required QOS threshold has been breached";
  case WAIT_QOS_JOB_LIMIT:
    return "QOS job limit reached";
  case WAIT_QOS_RESOURCE_LIMIT:
    return "QOS resource limit reached";
  case WAIT_QOS_TIME_LIMIT:
    return "QOS time limit reached";
  case WAIT_BLOCK_MAX_ERR:
    return "BLUEGENE Block has too many cnodes in error state to allow more "
           "jobs.";
  case WAIT_BLOCK_D_ACTION:
    return "BLUEGENE Block is being freed, can't allow more jobs.";
  case WAIT_CLEANING:
    return "If a job is requeued and it is still cleaning up from the last "
           "run.";
  case WAIT_PROLOG:
    return "Prolog is running";
  case WAIT_QOS:
    return "QOS not allowed";
  case WAIT_ACCOUNT:
    return "Account not allowed";
  case WAIT_DEP_INVALID:
    return "Dependency condition invalid or never satisfied";
  case WAIT_QOS_GRP_CPU:
    return "QOS GrpTRES exceeded (CPU)";
  case WAIT_QOS_GRP_CPU_MIN:
    return "QOS GrpTRESMins exceeded (CPU)";
  case WAIT_QOS_GRP_CPU_RUN_MIN:
    return "QOS GrpTRESRunMins exceeded (CPU)";
  case WAIT_QOS_GRP_JOB:
    return "QOS GrpJobs exceeded";
  case WAIT_QOS_GRP_MEM:
    return "QOS GrpTRES exceeded (Memory)";
  case WAIT_QOS_GRP_NODE:
    return "QOS GrpTRES exceeded (Node)";
  case WAIT_QOS_GRP_SUB_JOB:
    return "QOS GrpSubmitJobs exceeded";
  case WAIT_QOS_GRP_WALL:
    return "QOS GrpWall exceeded";
  case WAIT_QOS_MAX_CPU_PER_JOB:
    return "QOS MaxTRESPerJob exceeded (CPU)";
  case WAIT_QOS_MAX_CPU_MINS_PER_JOB:
    return "QOS MaxTRESMinsPerJob exceeded (CPU)";
  case WAIT_QOS_MAX_NODE_PER_JOB:
    return "QOS MaxTRESPerJob exceeded (Node)";
  case WAIT_QOS_MAX_WALL_PER_JOB:
    return "QOS MaxWallDurationPerJob exceeded";
  case WAIT_QOS_MAX_CPU_PER_USER:
    return "QOS MaxTRESPerUser exceeded (CPU)";
  case WAIT_QOS_MAX_JOB_PER_USER:
    return "QOS MaxJobsPerUser exceeded";
  case WAIT_QOS_MAX_NODE_PER_USER:
    return "QOS MaxTRESPerUser exceeded (Node)";
  case WAIT_QOS_MAX_SUB_JOB:
    return "QOS MaxSubmitJobsPerUser exceeded";
  case WAIT_QOS_MIN_CPU:
    return "QOS MinTRESPerJob not reached (CPU)";
  case WAIT_ASSOC_GRP_CPU:
    return "ASSOC GrpTRES exceeded (CPU)";
  case WAIT_ASSOC_GRP_CPU_MIN:
    return "ASSOC GrpTRESMins exceeded (CPU)";
  case WAIT_ASSOC_GRP_CPU_RUN_MIN:
    return "ASSOC GrpTRESRunMins exceeded (CPU)";
  case WAIT_ASSOC_GRP_JOB:
    return "ASSOC GrpJobs exceeded";
  case WAIT_ASSOC_GRP_MEM:
    return "ASSOC GrpTRES exceeded (Memory)";
  case WAIT_ASSOC_GRP_NODE:
    return "ASSOC GrpTRES exceeded (Node)";
  case WAIT_ASSOC_GRP_SUB_JOB:
    return "ASSOC GrpSubmitJobs exceeded";
  case WAIT_ASSOC_GRP_WALL:
    return "ASSOC GrpWall exceeded";
  case WAIT_ASSOC_MAX_JOBS:
    return "ASSOC MaxJobs exceeded";
  case WAIT_ASSOC_MAX_CPU_PER_JOB:
    return "ASSOC MaxTRESPerJob exceeded (CPU)";
  case WAIT_ASSOC_MAX_CPU_MINS_PER_JOB:
    return "ASSOC MaxTRESMinsPerJob exceeded (CPU)";
  case WAIT_ASSOC_MAX_NODE_PER_JOB:
    return "ASSOC MaxTRESPerJob exceeded (NODE)";
  case WAIT_ASSOC_MAX_WALL_PER_JOB:
    return "ASSOC MaxWallDurationPerJob exceeded";
  case WAIT_ASSOC_MAX_SUB_JOB:
    return "ASSOC MaxSubmitJobsPerUser exceeded";
  case WAIT_MAX_REQUEUE:
    return "MAX_BATCH_REQUEUE reached";
  case WAIT_ARRAY_TASK_LIMIT:
    return "job array running task limit";
  case WAIT_BURST_BUFFER_RESOURCE:
    return "Burst buffer resources";
  case WAIT_BURST_BUFFER_STAGING:
    return "Burst buffer file stage-in";
  case FAIL_BURST_BUFFER_OP:
    return "Burst buffer operation failure";
  case WAIT_POWER_NOT_AVAIL:
    return "not enough power available";
  case WAIT_POWER_RESERVED:
    return "job is waiting for available power because of power reservations";
  case WAIT_ASSOC_GRP_UNK:
    return "ASSOC GrpTRES exceeded (Unknown)";
  case WAIT_ASSOC_GRP_UNK_MIN:
    return "ASSOC GrpTRESMins exceeded (Unknown)";
  case WAIT_ASSOC_GRP_UNK_RUN_MIN:
    return "ASSOC GrpTRESRunMins exceeded (Unknown)";
  case WAIT_ASSOC_MAX_UNK_PER_JOB:
    return "ASSOC MaxTRESPerJob exceeded (Unknown)";
  case WAIT_ASSOC_MAX_UNK_PER_NODE:
    return "ASSOC MaxTRESPerNode exceeded (Unknown)";
  case WAIT_ASSOC_MAX_UNK_MINS_PER_JOB:
    return "ASSOC MaxTRESMinsPerJob exceeded (Unknown)";
  case WAIT_ASSOC_MAX_CPU_PER_NODE:
    return "ASSOC MaxTRESPerNode exceeded (CPU)";
  case WAIT_ASSOC_GRP_MEM_MIN:
    return "ASSOC GrpTRESMins exceeded (Memory)";
  case WAIT_ASSOC_GRP_MEM_RUN_MIN:
    return "ASSOC GrpTRESRunMins exceeded (Memory)";
  case WAIT_ASSOC_MAX_MEM_PER_JOB:
    return "ASSOC MaxTRESPerJob exceeded (Memory)";
  case WAIT_ASSOC_MAX_MEM_PER_NODE:
    return "ASSOC MaxTRESPerNode exceeded (CPU)";
  case WAIT_ASSOC_MAX_MEM_MINS_PER_JOB:
    return "ASSOC MaxTRESMinsPerJob exceeded (Memory)";
  case WAIT_ASSOC_GRP_NODE_MIN:
    return "ASSOC GrpTRESMins exceeded (Node)";
  case WAIT_ASSOC_GRP_NODE_RUN_MIN:
    return "ASSOC GrpTRESRunMins exceeded (Node)";
  case WAIT_ASSOC_MAX_NODE_MINS_PER_JOB:
    return "ASSOC MaxTRESMinsPerJob exceeded (Node)";
  case WAIT_ASSOC_GRP_ENERGY:
    return "ASSOC GrpTRES exceeded (Energy)";
  case WAIT_ASSOC_GRP_ENERGY_MIN:
    return "ASSOC GrpTRESMins exceeded (Energy)";
  case WAIT_ASSOC_GRP_ENERGY_RUN_MIN:
    return "ASSOC GrpTRESRunMins exceeded (Energy)";
  case WAIT_ASSOC_MAX_ENERGY_PER_JOB:
    return "ASSOC MaxTRESPerJob exceeded (Energy)";
  case WAIT_ASSOC_MAX_ENERGY_PER_NODE:
    return "ASSOC MaxTRESPerNode exceeded (Energy)";
  case WAIT_ASSOC_MAX_ENERGY_MINS_PER_JOB:
    return "ASSOC MaxTRESMinsPerJob exceeded (Energy)";
  case WAIT_ASSOC_GRP_GRES:
    return "ASSOC GrpTRES exceeded (GRES)";
  case WAIT_ASSOC_GRP_GRES_MIN:
    return "ASSOC GrpTRESMins exceeded (GRES)";
  case WAIT_ASSOC_GRP_GRES_RUN_MIN:
    return "ASSOC GrpTRESRunMins exceeded (GRES)";
  case WAIT_ASSOC_MAX_GRES_PER_JOB:
    return "ASSOC MaxTRESPerJob exceeded (GRES)";
  case WAIT_ASSOC_MAX_GRES_PER_NODE:
    return "ASSOC MaxTRESPerNode exceeded (GRES)";
  case WAIT_ASSOC_MAX_GRES_MINS_PER_JOB:
    return "ASSOC MaxTRESMinsPerJob exceeded (GRES)";
  case WAIT_ASSOC_GRP_LIC:
    return "ASSOC GrpTRES exceeded (license)";
  case WAIT_ASSOC_GRP_LIC_MIN:
    return "ASSOC GrpTRESMins exceeded (license)";
  case WAIT_ASSOC_GRP_LIC_RUN_MIN:
    return "ASSOC GrpTRESRunMins exceeded (license)";
  case WAIT_ASSOC_MAX_LIC_PER_JOB:
    return "ASSOC MaxTRESPerJob exceeded (license)";
  case WAIT_ASSOC_MAX_LIC_MINS_PER_JOB:
    return "ASSOC MaxTRESMinsPerJob exceeded (license)";
  case WAIT_ASSOC_GRP_BB:
    return "ASSOC GrpTRES exceeded (burst buffer)";
  case WAIT_ASSOC_GRP_BB_MIN:
    return "ASSOC GrpTRESMins exceeded (burst buffer)";
  case WAIT_ASSOC_GRP_BB_RUN_MIN:
    return "ASSOC GrpTRESRunMins exceeded (burst buffer)";
  case WAIT_ASSOC_MAX_BB_PER_JOB:
    return "ASSOC MaxTRESPerJob exceeded (burst buffer)";
  case WAIT_ASSOC_MAX_BB_PER_NODE:
    return "ASSOC MaxTRESPerNode exceeded (burst buffer)";
  case WAIT_ASSOC_MAX_BB_MINS_PER_JOB:
    return "ASSOC MaxTRESMinsPerJob exceeded (burst buffer)";
  case WAIT_QOS_GRP_UNK:
    return "QOS GrpTRES exceeded (Unknown)";
  case WAIT_QOS_GRP_UNK_MIN:
    return "QOS GrpTRESMins exceeded (Unknown)";
  case WAIT_QOS_GRP_UNK_RUN_MIN:
    return "QOS GrpTRESRunMins exceeded (Unknown)";
  case WAIT_QOS_MAX_UNK_PER_JOB:
    return "QOS MaxTRESPerJob exceeded (Unknown)";
  case WAIT_QOS_MAX_UNK_PER_NODE:
    return "QOS MaxTRESPerNode exceeded (Unknown)";
  case WAIT_QOS_MAX_UNK_PER_USER:
    return "QOS MaxTRESPerUser exceeded (Unknown)";
  case WAIT_QOS_MAX_UNK_MINS_PER_JOB:
    return "QOS MaxTRESMinsPerJob exceeded (Unknown)";
  case WAIT_QOS_MIN_UNK:
    return "QOS MinTRESPerJob exceeded (Unknown)";
  case WAIT_QOS_MAX_CPU_PER_NODE:
    return "QOS MaxTRESPerNode exceeded (CPU)";
  case WAIT_QOS_GRP_MEM_MIN:
    return "QOS GrpTRESMins exceeded (Memory)";
  case WAIT_QOS_GRP_MEM_RUN_MIN:
    return "QOS GrpTRESRunMins exceeded (Memory)";
  case WAIT_QOS_MAX_MEM_MINS_PER_JOB:
    return "QOS MaxTRESMinsPerJob exceeded (Memory)";
  case WAIT_QOS_MAX_MEM_PER_JOB:
    return "QOS MaxTRESPerJob exceeded (CPU)";
  case WAIT_QOS_MAX_MEM_PER_NODE:
    return "QOS MaxTRESPerNode exceeded (MEM)";
  case WAIT_QOS_MAX_MEM_PER_USER:
    return "QOS MaxTRESPerUser exceeded (CPU)";
  case WAIT_QOS_MIN_MEM:
    return "QOS MinTRESPerJob not reached (Memory)";
  case WAIT_QOS_GRP_ENERGY:
    return "QOS GrpTRES exceeded (Energy)";
  case WAIT_QOS_GRP_ENERGY_MIN:
    return "QOS GrpTRESMins exceeded (Energy)";
  case WAIT_QOS_GRP_ENERGY_RUN_MIN:
    return "QOS GrpTRESRunMins exceeded (Energy)";
  case WAIT_QOS_MAX_ENERGY_PER_JOB:
    return "QOS MaxTRESPerJob exceeded (Energy)";
  case WAIT_QOS_MAX_ENERGY_PER_NODE:
    return "QOS MaxTRESPerNode exceeded (Energy)";
  case WAIT_QOS_MAX_ENERGY_PER_USER:
    return "QOS MaxTRESPerUser exceeded (Energy)";
  case WAIT_QOS_MAX_ENERGY_MINS_PER_JOB:
    return "QOS MaxTRESMinsPerJob exceeded (Energy)";
  case WAIT_QOS_MIN_ENERGY:
    return "QOS MinTRESPerJob not reached (Energy)";
  case WAIT_QOS_GRP_NODE_MIN:
    return "QOS GrpTRESMins exceeded (Node)";
  case WAIT_QOS_GRP_NODE_RUN_MIN:
    return "QOS GrpTRESRunMins exceeded (Node)";
  case WAIT_QOS_MAX_NODE_MINS_PER_JOB:
    return "QOS MaxTRESMinsPerJob exceeded (Node)";
  case WAIT_QOS_MIN_NODE:
    return "QOS MinTRESPerJob not reached (Node)";
  case WAIT_QOS_GRP_GRES:
    return "QOS GrpTRES exceeded (GRES)";
  case WAIT_QOS_GRP_GRES_MIN:
    return "QOS GrpTRESMins exceeded (GRES)";
  case WAIT_QOS_GRP_GRES_RUN_MIN:
    return "QOS GrpTRESRunMins exceeded (GRES)";
  case WAIT_QOS_MAX_GRES_PER_JOB:
    return "QOS MaxTRESPerJob exceeded (GRES)";
  case WAIT_QOS_MAX_GRES_PER_NODE:
    return "QOS MaxTRESPerNode exceeded (GRES)";
  case WAIT_QOS_MAX_GRES_PER_USER:
    return "QOS MaxTRESPerUser exceeded (GRES)";
  case WAIT_QOS_MAX_GRES_MINS_PER_JOB:
    return "QOS MaxTRESMinsPerJob exceeded (GRES)";
  case WAIT_QOS_MIN_GRES:
    return "QOS MinTRESPerJob not reached (CPU)";
  case WAIT_QOS_GRP_LIC:
    return "QOS GrpTRES exceeded (license)";
  case WAIT_QOS_GRP_LIC_MIN:
    return "QOS GrpTRESMins exceeded (license)";
  case WAIT_QOS_GRP_LIC_RUN_MIN:
    return "QOS GrpTRESRunMins exceeded (license)";
  case WAIT_QOS_MAX_LIC_PER_JOB:
    return "QOS MaxTRESPerJob exceeded (license)";
  case WAIT_QOS_MAX_LIC_PER_USER:
    return "QOS MaxTRESPerUser exceeded (license)";
  case WAIT_QOS_MAX_LIC_MINS_PER_JOB:
    return "QOS MaxTRESMinsPerJob exceeded (license)";
  case WAIT_QOS_MIN_LIC:
    return "QOS MinTRESPerJob not reached (license)";
  case WAIT_QOS_GRP_BB:
    return "QOS GrpTRES exceeded (burst buffer)";
  case WAIT_QOS_GRP_BB_MIN:
    return "QOS GrpTRESMins exceeded (burst buffer)";
  case WAIT_QOS_GRP_BB_RUN_MIN:
    return "QOS GrpTRESRunMins exceeded (burst buffer)";
  case WAIT_QOS_MAX_BB_PER_JOB:
    return "QOS MaxTRESPerJob exceeded (burst buffer)";
  case WAIT_QOS_MAX_BB_PER_NODE:
    return "QOS MaxTRESPerNode exceeded (burst buffer)";
  case WAIT_QOS_MAX_BB_PER_USER:
    return "QOS MaxTRESPerUser exceeded (burst buffer)";
  case WAIT_QOS_MAX_BB_MINS_PER_JOB:
    return "QOS MaxTRESMinsPerJob exceeded (burst buffer)";
  case WAIT_QOS_MIN_BB:
    return "QOS MinTRESPerJob not reached (burst buffer)";
  case FAIL_DEADLINE:
    return "reached deadline";
  case WAIT_QOS_MAX_BB_PER_ACCT:
    return "exceeded burst buffer";
  case WAIT_QOS_MAX_CPU_PER_ACCT:
    return "exceeded CPUs";
  case WAIT_QOS_MAX_ENERGY_PER_ACCT:
    return "exceeded Energy";
  case WAIT_QOS_MAX_GRES_PER_ACCT:
    return "exceeded GRES";
  case WAIT_QOS_MAX_NODE_PER_ACCT:
    return "exceeded Nodes";
  case WAIT_QOS_MAX_LIC_PER_ACCT:
    return "exceeded Licenses";
  case WAIT_QOS_MAX_MEM_PER_ACCT:
    return "exceeded Memory";
  case WAIT_QOS_MAX_UNK_PER_ACCT:
    return "exceeded Unknown";
  case WAIT_QOS_MAX_JOB_PER_ACCT:
    return "QOS MaxJobPerAccount exceeded";
  case WAIT_QOS_MAX_SUB_JOB_PER_ACCT:
    return "QOS MaxJobSubmitSPerAccount exceeded";
  case WAIT_PART_CONFIG:
    return "Generic partition configuration reason";
  case WAIT_ACCOUNT_POLICY:
    return "Generic accounting policy reason";
  case WAIT_FED_JOB_LOCK:
    return "Can't get fed job lock";
  case FAIL_OOM:
    return "Exhausted memory";
  case WAIT_PN_MEM_LIMIT:
    return "MaxMemPer[CPU|Node] exceeded";
  case WAIT_ASSOC_GRP_BILLING:
    return "GrpTRES          ";
  case WAIT_ASSOC_GRP_BILLING_MIN:
    return "GrpTRESMins      ";
  case WAIT_ASSOC_GRP_BILLING_RUN_MIN:
    return "GrpTRESRunMins   ";
  case WAIT_ASSOC_MAX_BILLING_PER_JOB:
    return "MaxTRESPerJob    ";
  case WAIT_ASSOC_MAX_BILLING_PER_NODE:
    return "MaxTRESPerNode   ";
  case WAIT_ASSOC_MAX_BILLING_MINS_PER_JOB:
    return "MaxTRESMinsPerJob";
  case WAIT_QOS_GRP_BILLING:
    return "GrpTRES          ";
  case WAIT_QOS_GRP_BILLING_MIN:
    return "GrpTRESMins      ";
  case WAIT_QOS_GRP_BILLING_RUN_MIN:
    return "GrpTRESRunMins   ";
  case WAIT_QOS_MAX_BILLING_PER_JOB:
    return "MaxTRESPerJob    ";
  case WAIT_QOS_MAX_BILLING_PER_NODE:
    return "MaxTRESPerNode   ";
  case WAIT_QOS_MAX_BILLING_PER_USER:
    return "MaxTRESPerUser   ";
  case WAIT_QOS_MAX_BILLING_MINS_PER_JOB:
    return "MaxTRESMinsPerJob";
  case WAIT_QOS_MAX_BILLING_PER_ACCT:
    return "MaxTRESPerAcct   ";
  case WAIT_QOS_MIN_BILLING:
    return "MinTRESPerJob    ";
  default:
    return "<Unknown>";
  }
}

void print_jobs(FILE *file) {
  job_info_msg_t *job_buffer_ptr = NULL;

  /* get and dump some job information */
  if (slurm_load_jobs((time_t)NULL, &job_buffer_ptr, SHOW_ALL)) {
    slurm_perror("slurm_load_jobs error");
    exit(1);
  }

  fprintf(file, "ACCOUNT\t"
                "ACCRUE_TIME\t"
                "ADMIN_COMMENT\t"
                "ALLOCATE_NODE\t"
                "ALLOCATE_SID\t"
                "ARRAY_JOB_ID\t"
                "ARRAY_TASK_ID\t"
                "ARRAY_MAX_TASKS\t"
                "ARRAY_TASK\t"
                "ASSOCIATION_ID\t"
                "BATCH_FEATURES\t"
                "BATCH_FLAG\t"
                "BATCH_HOST\t"
                "FLAGS\t"
                "BOARDS_PER_NODE\t"
                "BURST_BUFFER\t"
                "BURST_BUFFER_STATE\t"
                "CLUSTER\t"
                "CLUSTER_FEATURES\t"
                "COMMAND\t"
                "COMMENT\t"
                "CONTIGUOUS\t"
                "CORE_SPEC\t"
                "CORES_PER_SOCKET\t"
                "BILLABLE_TRES\t"
                "CPU_PER_TASK\t"
                "CPU_FREQ_MIN\t"
                "CPU_FREQ_MAX\t"
                "CPU_FREQ_GOVERNOR\t"
                "CPUS_PER_TRES\t"
                "DEADLINE\t"
                "DELAY_BOOT\t"
                "DEPENDENCY\t"
                "DERIVED_EC\t"
                "ELIGIBLE_TIME\t"
                "END_TIME\t"
                "EXC_NODES\t"
                "EXIT_CODE\t"
                "FEATURES\t"
                "FED_ORIGIN\t"
                "RED_SIBLINGS_ACTIVE\t"
                "RED_SIBLINGS_VIABLE\t"
                "GRES_DETAIL\t"
                "GRES_TOTAL\t"
                "GROUP_ID\t"
                "GROUP_NAME\t"
                "HET_JOB_ID\t"
                "HET_JOB_ID_SET\t"
                "HET_JOB_OFFSET\t"
                "JOB_ID\t"
                "JOB_STATE\t"
                "LAST_SCHEDULE_EVALUATE\t"
                "LICENSES\t"
                "MAIL_TYPE\t"
                "MAIL_USER\t"
                "MAX_CPUS\t"
                "MAX_NODES\t"
                "MCS_LABEL\t"
                "MEM_PER_TRES\t"
                "NAME\t"
                "NETWORK\t"
                "NODES\t"
                "NICE\t"
                "NTASKS_PER_CORE\t"
                "NTASKS_PER_NODE\t"
                "NTASKS_PER_SOCKET\t"
                "NTASKS_PER_BOARD\t"
                "NUM_CPUS\t"
                "NUM_NODES\t"
                "NUM_TASKS\t"
                "PARTITION\t"
                "MIN_MEMORY_PER_NODE\t"
                "MIN_CPUS_PER_NODE\t"
                "MIN_TMP_DISK_PER_NODE\t"
                "POWER_FLAGS\t"
                "PREEMPT_TIME\t"
                "PREEMPTABLE_TIME\t"
                "PRE_SUSPEND_TIME\t"
                "PRIORITY\t"
                "PROFILE\t"
                "QOS\t"
                "REBOOT\t"
                "REQUESTED_NODES\t"
                "REQUESTED_SWITCH_NUM\t"
                "REQUEUE\t"
                "RESIZE_TIME\t"
                "RESTART_COUNT\t"
                "RESERVATION_NAME\t"
                "SCHEDULED_NODES\t"
                "SHARED\t"
                "SHOW_FLAGS\t"
                "SITE_FACTOR\t"
                "SOCKETS_PER_BOARD\t"
                "SOCKETS_PER_NODE\t"
                "START_TIME\t"
                "START_PROTOCOL_VER\t"
                "STATE_DESC\t"
                "STATE_REASON_ID\t"
                "STDERR\t"
                "STDOUT\t"
                "STDIN\t"
                "SUBMIT_TIME\t"
                "SUSPEND_TIME\t"
                "SYSTEM_COMMENT\t"
                "TIME_LIMIT\t"
                "TIME_MIN\t"
                "THREADS_PER_CORE\t"
                "TRES_BIND\t"
                "TRES_FREQ\t"
                "TRES_PER_JOB\t"
                "TRES_PER_NODE\t"
                "TRES_PER_SOCKET\t"
                "TRES_PER_TASK\t"
                "TRES_REQ\t"
                "TRES_ALLOC\t"
                "USER_ID\t"
                "USERNAME\t"
                "USERNAME_RESOLVED\t"
                "WAIT4SWITCH\t"
                "WCKEY\t"
                "WORKDIR\n");

  for (size_t i = 0; i < job_buffer_ptr->record_count; i++) {
    slurm_job_info_t *job_ptr = &job_buffer_ptr->job_array[i];
    fprintf(file, "%s\t", job_ptr->account);
    print_time(file, job_ptr->accrue_time, false);
    fprintf(file, "%s\t", job_ptr->admin_comment);
    fprintf(file, "%s\t", job_ptr->alloc_node);
    fprintf(file, "%u\t", job_ptr->alloc_sid);
    if (job_ptr->array_job_id != 0) {
      print_u32(file, job_ptr->array_job_id, false);
    } else {
      fprintf(file, "(null)\t");
    }
    print_u32(file, job_ptr->array_task_id, false);
    print_u32(file, job_ptr->array_max_tasks, false);
    fprintf(file, "%s\t", job_ptr->array_task_str);
    print_u32(file, job_ptr->assoc_id, false);
    fprintf(file, "%s\t", job_ptr->batch_features);
    fprintf(file, "%s\t", job_ptr->batch_flag ? "Yes" : "No");
    fprintf(file, "%s\t", job_ptr->batch_host);
    print_job_flag(file, job_ptr->bitflags);
    fprintf(file, "\t");
    print_u16(file, job_ptr->boards_per_node, false);
    fprintf(file, "%s\t", job_ptr->burst_buffer);
    fprintf(file, "%s\t", job_ptr->burst_buffer_state);
    fprintf(file, "%s\t", job_ptr->cluster);
    fprintf(file, "%s\t", job_ptr->cluster_features);
    fprintf(file, "%s\t", job_ptr->command);
    fprintf(file, "%s\t", job_ptr->comment);
    fprintf(file, "%s\t", job_ptr->contiguous ? "Yes" : "No");
    print_u16(file, job_ptr->core_spec, false);
    print_u16(file, job_ptr->cores_per_socket, false);
    fprintf(file, "%.2lf\t", job_ptr->billable_tres);
    print_u16(file, job_ptr->cpus_per_task, false);
    print_u32(file, job_ptr->cpu_freq_min, false);
    print_u32(file, job_ptr->cpu_freq_max, false);
    print_u32(file, job_ptr->cpu_freq_gov, false);
    fprintf(file, "%s\t", job_ptr->cpus_per_tres);
    print_time(file, job_ptr->deadline, false);
    print_u32(file, job_ptr->delay_boot, false);
    fprintf(file, "%s\t", job_ptr->dependency);
    print_u32(file, job_ptr->derived_ec, false);
    print_time(file, job_ptr->eligible_time, false);
    print_time(file, job_ptr->end_time, false);
    fprintf(file, "%s\t", job_ptr->exc_nodes);
    print_u32(file, job_ptr->exit_code, false);
    fprintf(file, "%s\t", job_ptr->features);
    fprintf(file, "%s\t", job_ptr->fed_origin_str);
    fprintf(file, "%s\t", job_ptr->fed_siblings_active_str);
    fprintf(file, "%s\t", job_ptr->fed_siblings_viable_str);
    for (uint32_t i = 0; i < job_ptr->gres_detail_cnt; i++) {
      if (i != 0) {
        fprintf(file, ",");
      }
      fprintf(file, "%s", job_ptr->gres_detail_str[i]);
    }
    fprintf(file, "\t");
    fprintf(file, "%s\t", job_ptr->gres_total);
    print_u32(file, job_ptr->group_id, false);

    struct group *grp = getgrgid(job_ptr->group_id);
    if (grp) {
      fprintf(file, "%s\t", grp->gr_name);
    } else {
      fprintf(file, "<unknown>\t");
    }

    print_u32(file, job_ptr->het_job_id, false);
    fprintf(file, "%s\t", job_ptr->het_job_id_set);
    print_u32(file, job_ptr->het_job_offset, false);
    print_u32(file, job_ptr->job_id, false);
    print_job_state(file, job_ptr->job_state);
    fprintf(file, "\t");
    print_time(file, job_ptr->last_sched_eval, false);
    fprintf(file, "%s\t", job_ptr->licenses);
    print_u16(file, job_ptr->mail_type, false); // TODO: humanize data
    fprintf(file, "%s\t", job_ptr->mail_user);
    print_u32(file, job_ptr->max_cpus, false);
    print_u32(file, job_ptr->max_nodes, false);
    fprintf(file, "%s\t", job_ptr->mcs_label);
    fprintf(file, "%s\t", job_ptr->mem_per_tres);
    fprintf(file, "%s\t", job_ptr->name);
    fprintf(file, "%s\t", job_ptr->network);
    fprintf(file, "%s\t", job_ptr->nodes);
    print_u32(file, job_ptr->nice, false);
    print_u16(file, job_ptr->ntasks_per_core, false);
    print_u16(file, job_ptr->ntasks_per_node, false);
    print_u16(file, job_ptr->ntasks_per_socket, false);
    print_u16(file, job_ptr->ntasks_per_board, false);
    print_u32(file, job_ptr->num_cpus, false);
    print_u32(file, job_ptr->num_nodes, false);
    print_u32(file, job_ptr->num_tasks, false);
    fprintf(file, "%s\t", job_ptr->partition);
    print_u64(file, job_ptr->pn_min_memory, false);
    print_u16(file, job_ptr->pn_min_cpus, false);
    print_u32(file, job_ptr->pn_min_tmp_disk, false);
    fprintf(file, "%x\t", job_ptr->power_flags);
    print_time(file, job_ptr->preempt_time, false);
    print_time(file, job_ptr->preemptable_time, false);
    print_time(file, job_ptr->pre_sus_time, false);
    print_u32(file, job_ptr->priority, false);
    print_u32(file, job_ptr->profile, false);
    fprintf(file, "%s\t", job_ptr->qos);
    fprintf(file, "%s\t", job_ptr->reboot ? "Yes" : "No");
    fprintf(file, "%s\t", job_ptr->req_nodes);
    print_u32(file, job_ptr->req_switch, false);
    fprintf(file, "%s\t", job_ptr->requeue ? "Yes" : "No");
    print_time(file, job_ptr->resize_time, false);
    print_u16(file, job_ptr->restart_cnt, false);
    fprintf(file, "%s\t", job_ptr->resv_name);
    fprintf(file, "%s\t", job_ptr->sched_nodes);
    fprintf(file, "%s\t", job_ptr->shared ? "Yes" : "No");
    print_u16(file, job_ptr->show_flags, false);
    print_u32(file, job_ptr->site_factor, false);
    print_u16(file, job_ptr->sockets_per_board, false);
    print_u16(file, job_ptr->sockets_per_node, false);
    print_time(file, job_ptr->start_time, false);
    print_u16(file, job_ptr->start_protocol_ver, false);
    fprintf(file, "%s\t", job_ptr->state_desc);
    fprintf(file, "%s\t", get_reason_text(job_ptr->state_reason));
    fprintf(file, "%s\t", job_ptr->std_err);
    fprintf(file, "%s\t", job_ptr->std_out);
    fprintf(file, "%s\t", job_ptr->std_in);
    print_time(file, job_ptr->submit_time, false);
    print_time(file, job_ptr->suspend_time, false);
    fprintf(file, "%s\t", job_ptr->system_comment);
    print_u32(file, job_ptr->time_limit, false);
    print_u32(file, job_ptr->time_min, false);
    print_u16(file, job_ptr->threads_per_core, false);
    fprintf(file, "%s\t", job_ptr->tres_bind);
    fprintf(file, "%s\t", job_ptr->tres_freq);
    fprintf(file, "%s\t", job_ptr->tres_per_job);
    fprintf(file, "%s\t", job_ptr->tres_per_node);
    fprintf(file, "%s\t", job_ptr->tres_per_socket);
    fprintf(file, "%s\t", job_ptr->tres_per_task);
    fprintf(file, "%s\t", job_ptr->tres_req_str);
    fprintf(file, "%s\t", job_ptr->tres_alloc_str);
    print_u32(file, job_ptr->user_id, false);
    fprintf(file, "%s\t", job_ptr->user_name);

    struct passwd *pwd = getpwuid(job_ptr->user_id);
    if (pwd) {
      fprintf(file, "%s\t", pwd->pw_name);
    } else {
      fprintf(file, "<unknown>\t");
    }

    print_u32(file, job_ptr->wait4switch, false);
    fprintf(file, "%s\t", job_ptr->wckey);
    fprintf(file, "%s", job_ptr->work_dir);
    fprintf(file, "\n");
  }
}
