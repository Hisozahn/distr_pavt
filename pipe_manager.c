#include "pipe_manager.h"

int set_nonblock(int pipefd[2]) {
	int flags_read = fcntl(pipefd[0], F_GETFL, 0);
	int flags_write = fcntl(pipefd[0], F_GETFL, 0);
	if (fcntl (pipefd[0], F_SETFL, flags_read | O_NONBLOCK) < 0) {
		perror("fcntl");
		return -1;
	}
	if (fcntl (pipefd[1], F_SETFL, flags_write | O_NONBLOCK) < 0) {
		perror("fcntl");
		return -1;
	}
	return 0;
}

int init_pipe_ios(pipe_io_ptr pipe_ios, int8_t process_count) {
	for (int8_t i = 0; i < process_count; i++) {
		pipe_ios[i].process_count = process_count;
	}
	int pipefd[2];
	int pipes_log_fd = open(pipes_log, O_CREAT | O_APPEND | O_WRONLY, 0666);
	if (pipes_log_fd < 0) {
		return -1;
	}
	
	for (int8_t process_from = 0; process_from < process_count; process_from++) {
		for (int8_t process_to = 0; process_to < process_count; process_to++) {

			if (process_from == process_to) {
				pipe_ios[process_to].read_fd[process_from] = PROHIBITED_FD;
				pipe_ios[process_from].write_fd[process_to] = PROHIBITED_FD;
				continue;
			}
			if (pipe(pipefd) == -1) {
				perror("pipe call");
				return -1;
			}
			
			if (set_nonblock(pipefd) < 0) {
				return -1;
			}
			
			pipe_ios[process_to].read_fd[process_from] = pipefd[0];
			pipe_ios[process_from].write_fd[process_to] = pipefd[1];
			
			if (log_pipe_descriptors(pipefd, pipes_log_fd) < 0) {
				return -1;
			}
		}
	}

	if( close(pipes_log_fd) != 0) {
		return -1;
	}
	
	return 0;
}

int close_pipe_fds(const int *pipe_fds) {
	int status = 0;

	for (int8_t i = 0; i < MAX_PROCESS_COUNT; i++) {
		if (pipe_fds[i] == 0 || pipe_fds[i] == PROHIBITED_FD) {
			continue;
		}
		if (close(pipe_fds[i]) < 0) {
			perror("close error");
			status = -1;
		}
	}

	return status;
}

int destroy_pipe_ios(const pipe_io_ptr pipe_ios, int8_t except_local_id) {
	int status = 0;

	for (int8_t i = 0; i < MAX_PROCESS_COUNT; i++) {
		if (pipe_ios[i].process_count == 0 || i == except_local_id) {
			continue;
		}
		if (close_pipe_fds(pipe_ios[i].write_fd) < 0
			|  close_pipe_fds(pipe_ios[i].read_fd) < 0) {
			status = -1;
		}
	}
	
	return status;
}
