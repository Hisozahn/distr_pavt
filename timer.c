#include "timer.h"

static timestamp_t lamport_time = 0;

timestamp_t get_lamport_time() {
	return lamport_time;
}

void advance_lamport_time(timestamp_t time_from_msg) {
	lamport_time = time_from_msg > lamport_time ? time_from_msg : lamport_time;
	lamport_time++;
}
