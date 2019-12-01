#include "parent_process.h"

void terminate_child_processes(const pid_t *child_pids) {
	for (int8_t i = 0; i < MAX_CHILD_COUNT; i++) {
		if (0 == child_pids[i]) {
			continue;
		}
		kill(child_pids[i], SIGKILL);
	}
}

int parent_lifecycle(const pipe_io_ptr pipe_io, const pid_t *child_pids, int events_log_fd) {
	Message msg_started;

    init_vector_time(0);

	for (int8_t i = 1; i < pipe_io->process_count; i++) {
		if (receive(pipe_io, i, &msg_started) < 0) {
			return EXIT_FAILURE;
		}
		advance_vector_time(msg_started.s_header.s_local_timevector);
	}
	
	if (log_recieved_all_started(events_log_fd, get_vector_time(), PARENT_ID) < 0) {
		return EXIT_FAILURE;
	}
	
	bank_robbery(pipe_io, pipe_io->process_count - 1);
	
	Message msg_stop;
	advance_vector_time(TIME_OMITTED);
	init_stop_msg(&msg_stop);

	if (send_multicast(pipe_io, &msg_stop) < 0) {
		fputs("send_multicast failed", stderr);
		return EXIT_FAILURE;
	}
	
	Message msg_done;
	for (int8_t i = 1; i < pipe_io->process_count; i++) {
		if (receive(pipe_io, i, &msg_done) < 0) {
			return EXIT_FAILURE;
		}
		advance_vector_time(msg_done.s_header.s_local_timevector);
	}
	
	if (log_recieved_all_done(events_log_fd, get_vector_time(), PARENT_ID) < 0) {
		return EXIT_FAILURE;
	}
	
	if( close(events_log_fd) != 0) {
		return EXIT_FAILURE;
	}
	
	if (close_pipe_fds(pipe_io->write_fd) < 0
		| close_pipe_fds(pipe_io->read_fd) < 0) {
		terminate_child_processes(child_pids);
		return EXIT_FAILURE;
	}
	
	while(wait(NULL)) {
		if (errno == ECHILD) {
			break;
		}
	}
	
	return 0;
}
