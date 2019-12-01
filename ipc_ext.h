#ifndef __IPC_EXT__H
#define __IPC_EXT__H

#include <sys/types.h>
#include <stdio.h>

#include "definitions.h"
#include "ipc.h"
#include "pavt.h"
#include "stdbool.h"
#include "logger.h"
#include "banking.h"
#include "ipc.h"

bool is_valid_started_msg(const Message *msg);
bool is_valid_done_msg(const Message *msg);

int init_started_msg(Message *msg, int8_t local_id, pid_t pid, pid_t parent_pid, balance_t balance);
int init_done_msg(Message *msg, int8_t local_id, balance_t balance);
void init_transfer_msg(Message *msg, const TransferOrder *order);
void copy_transfer_msg(Message *dst, const Message *src);
void init_stop_msg(Message *msg);
void init_ack_msg(Message *msg); 
void init_balance_msg(Message *msg, balance_t balance, timestamp_t *vec_time);
void init_vtime_msg(Message *msg, timestamp_t vec_time);
void init_snap_ack_msg(Message *msg);
void init_empty_msg(Message *msg);

#endif // __IPC_EXT__H
