#include "ipc.h"
#include "parent_process.h"

int send(void * self, local_id dst, const Message * msg) {
	pipe_io_ptr pipe_io = (pipe_io_ptr)self;

	if (pipe_io->write_fd[dst] == PROHIBITED_FD) {
		return -1;
	}

	size_t message_length = sizeof(msg->s_header) + msg->s_header.s_payload_len;
	if (write(pipe_io->write_fd[dst], msg, message_length) < 0) {
		if (EAGAIN == errno) {
			perror("send failed");
			return -1;
		}
		else {
			perror("send failed");
			return -1;
		}
	}
	else {
		return 0;
	}
}

int send_multicast(void * self, const Message * msg) {
	pipe_io_ptr pipe_io = (pipe_io_ptr)self;

	for (int8_t i = 0; i < pipe_io->process_count; i++) {
		if (pipe_io->write_fd[i] == PROHIBITED_FD) {
			continue;
		}
		
		size_t message_length = sizeof(msg->s_header) + msg->s_header.s_payload_len;
		if (write(pipe_io->write_fd[i], msg, message_length) < 0) {
			if (EAGAIN == errno) {
				perror("send failed");
				return -1;
			}
			else {
				perror("send failed");
				return -1;
			}
		}
	}
	return 0;
}

int receive(void * self, local_id from, Message * msg) {
	pipe_io_ptr pipe_io = (pipe_io_ptr)self;
	if (pipe_io->read_fd[from] == PROHIBITED_FD) {
		return -1;
	}

	while (1) {
		MessageHeader header;
		ssize_t readed_count;
		if ((readed_count = read(pipe_io->read_fd[from], &header, sizeof(msg->s_header))) <= 0) {
			if (EAGAIN == errno || 0 == readed_count) {
				continue;
			}
			else {
				perror("receive failed");
				return -1;
			}
		}
		else {
			msg->s_header = header;
			if (read(pipe_io->read_fd[from], msg->s_payload, header.s_payload_len) < 0) {
				perror("receive failed");
				return -1;
			}
			else {
				return EXIT_SUCCESS;
			}
		}
	}
}

int receive_any(void * self, Message * msg) {
	pipe_io_ptr pipe_io = (pipe_io_ptr)self;
	
	for (int8_t i = 0; i < pipe_io->process_count; i++) {
		if (pipe_io->write_fd[i] == PROHIBITED_FD) {
			continue;
		}
		MessageHeader header;
		ssize_t readed_count;
		if ((readed_count = read(pipe_io->read_fd[i], &header, sizeof(msg->s_header))) <= 0) {
			if (EAGAIN == errno || 0 == readed_count ) {
				continue;
			}
			else {
				perror("receive any failed");
				return -1;
			}
		}
		else {
			msg->s_header = header;
			if (0 == header.s_payload_len) {
				return i;
			}
			if (read(pipe_io->read_fd[i], msg->s_payload, header.s_payload_len) <= 0) {
				perror("receive any failed");
				return -1;
			}
			else {
				return i;
			}
		}
	}
	return -2;
}
