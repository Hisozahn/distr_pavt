#include "logger.h"

int log_pipe_descriptors(int *pipefds, int pipes_log_fd) {
	char buffer[MAX_LOG_STRING_LENGTH];
	int out_byte_count;
	if ( (out_byte_count = snprintf(buffer, sizeof(buffer) / sizeof(buffer[0])
			, "pipe: readfd = %d, writefd = %d\n", pipefds[0], pipefds[1])) < 0) {
		return -1;
	}
	if (write(pipes_log_fd, buffer, out_byte_count) < 0
			| write(STDOUT_FILENO, buffer, out_byte_count) < 0) {
		perror("logger: write");
		return -1;
	}

	return 0;
}

int tee_output(int events_log_fd, char *buffer, int out_byte_count) {
	if (write(events_log_fd, buffer, out_byte_count) < 0
			| write(STDOUT_FILENO, buffer, out_byte_count) < 0) {
		return -1;
	}
	return 0;
}

int log_started(int events_log_fd, timestamp_t timestamp, local_id process, pid_t pid, pid_t parent_pid, balance_t balance) {
	char buffer[MAX_LOG_STRING_LENGTH];
	size_t buffer_size = sizeof(buffer) / sizeof(buffer[0]);
	
	int out_byte_count = snprintf(buffer, buffer_size, log_started_fmt,
			timestamp, process, pid, parent_pid, balance);
	
	return tee_output(events_log_fd, buffer, out_byte_count);
}

int log_done(int events_log_fd, timestamp_t timestamp, local_id process, balance_t balance) {
	char buffer[MAX_LOG_STRING_LENGTH];
	size_t buffer_size = sizeof(buffer) / sizeof(buffer[0]);
	
	int out_byte_count = snprintf(buffer, buffer_size, log_done_fmt,
			timestamp, process, balance);
	
	return tee_output(events_log_fd, buffer, out_byte_count);
}

int log_recieved_all_started(int events_log_fd, timestamp_t timestamp, local_id process) {
	char buffer[MAX_LOG_STRING_LENGTH];
	size_t buffer_size = sizeof(buffer) / sizeof(buffer[0]);
	
	int out_byte_count = snprintf(buffer, buffer_size, log_received_all_started_fmt,
			timestamp, process);
	
	return tee_output(events_log_fd, buffer, out_byte_count);
}

int log_recieved_all_done(int events_log_fd, timestamp_t timestamp, local_id process) {
	char buffer[MAX_LOG_STRING_LENGTH];
	size_t buffer_size = sizeof(buffer) / sizeof(buffer[0]);
	
	int out_byte_count = snprintf(buffer, buffer_size, log_received_all_done_fmt,
			timestamp, process);
	
	return tee_output(events_log_fd, buffer, out_byte_count);
}

int log_transfer_out(int events_log_fd, timestamp_t timestamp, local_id process, balance_t balance, local_id process_to) {
	char buffer[MAX_LOG_STRING_LENGTH];
	size_t buffer_size = sizeof(buffer) / sizeof(buffer[0]);
	
	int out_byte_count = snprintf(buffer, buffer_size, log_transfer_out_fmt,
			timestamp, process, balance, process_to);
	
	return tee_output(events_log_fd, buffer, out_byte_count);
}

int log_transfer_in(int events_log_fd, timestamp_t timestamp, local_id process, balance_t balance, local_id process_from) {
	char buffer[MAX_LOG_STRING_LENGTH];
	size_t buffer_size = sizeof(buffer) / sizeof(buffer[0]);
	
	int out_byte_count = snprintf(buffer, buffer_size, log_transfer_in_fmt,
			timestamp, process, balance, process_from);
	
	return tee_output(events_log_fd, buffer, out_byte_count);
}
