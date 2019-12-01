#ifndef __LOGGER__H
#define __LOGGER__H

#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "pa2345.h"
#include "banking.h"

typedef enum {
	STARTED_EVENT,
	RECEIVED_ALL_STARTED_EVENT,
	DONE_EVENT,
	RECEIVED_ALL_DONE_EVENT
} event_type;

static const int MAX_LOG_STRING_LENGTH = 100;

int log_pipe_descriptors(int *pipefds, int pipes_log_fd);
int log_started(int events_log_fd, timestamp_t timestamp, local_id process, pid_t pid, pid_t parent_pid, balance_t balance);
int log_done(int events_log_fd, timestamp_t timestamp, local_id process, balance_t balance);
int log_recieved_all_started(int events_log_fd, timestamp_t timestamp, local_id process);
int log_recieved_all_done(int events_log_fd, timestamp_t timestamp, local_id process);
int log_transfer_out(int events_log_fd, timestamp_t timestamp, local_id process, balance_t balance, local_id process_to);
int log_transfer_in(int events_log_fd, timestamp_t timestamp, local_id process, balance_t balance, local_id process_from);


#endif // __LOGGER__H
