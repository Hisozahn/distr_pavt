#ifndef __PARENT_PROCESS__H
#define __PARENT_PROCESS__H

#define _POSIX_SOURCE

#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>

#include "definitions.h"
#include "pipe_manager.h"
#include "ipc.h"
#include "ipc_ext.h"
#include "banking.h"

void terminate_child_processes(const pid_t *child_pids);
int parent_lifecycle(const pipe_io_ptr pipe_io, const pid_t *child_pids, int events_log_fd);

#endif // __PARENT_PROCESS__H
