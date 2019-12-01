#ifndef TIMER_H
#define TIMER_H

#include "ipc.h"
typedef struct timevector_t {
    timestamp_t vec[MAX_PROCESS_ID];
} timevector_t;

static const timestamp_t TIME_OMITTED[MAX_PROCESS_ID] = {0};

void init_vector_time(unsigned int id);
timestamp_t get_vector_time();
timevector_t get_vector_timevec();
void advance_vector_time(const timestamp_t * time_from_msg);

#endif /* TIMER_H */

