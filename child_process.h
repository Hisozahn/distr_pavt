#ifndef __CHILD_PROCESS__H
#define __CHILD_PROCESS__H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

#include "definitions.h"
#include "ipc.h"
#include "ipc_ext.h"
#include "pipe_manager.h"
#include "timer.h"

typedef enum {
    WORK = 0,
    END_WORK = 1
} work_stage;

int child_lifecycle(const pipe_io_ptr pipe_io, int8_t local_id, pid_t parent_pid, int events_log_fd, balance_t start_balance);

#endif // __CHILD_PROCESS__H
