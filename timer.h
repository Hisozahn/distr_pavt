#ifndef TIMER_H
#define TIMER_H

#include "ipc.h"

static const timestamp_t TIME_OMITTED = -1;

timestamp_t get_lamport_time();
void advance_lamport_time(timestamp_t time_from_msg);

#endif /* TIMER_H */

