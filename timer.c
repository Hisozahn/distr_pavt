#include "timer.h"

static timevector_t vector_time = {.vec = {0}};
static unsigned int proc_id;

void init_vector_time(unsigned int id)
{
    proc_id = id;
}

timestamp_t get_vector_time() {
	return vector_time.vec[proc_id];
}

timevector_t get_vector_timevec() {
	return vector_time;
}

void advance_vector_time(const timestamp_t * time_from_msg)
{
    for (int i = 0; i < MAX_PROCESS_ID; i ++)
    {
        vector_time.vec[i] = time_from_msg[i] > vector_time.vec[i] ?
                time_from_msg[i] : vector_time.vec[i];
    }
	vector_time.vec[proc_id]++;
}
