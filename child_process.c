#include "child_process.h"

void get_order_from_msg(const Message *msg, TransferOrder *order) {
	memcpy(order, msg->s_payload, sizeof(*order));
}

int work_loop( pipe_io_ptr pipe_io, int events_log_fd
				, balance_t *current_balance, int8_t local_id) {
	int8_t done_count = 0;
	work_stage stage = WORK;
	while (1) {
		Message msg_work;
		if (receive_any(pipe_io, &msg_work) < 0) {
			return -1;
		}
		advance_vector_time(msg_work.s_header.s_local_timevector);
		
		if (TRANSFER == msg_work.s_header.s_type) {
			TransferOrder order;
			get_order_from_msg(&msg_work, &order);
			
					
			if (order.s_src == local_id) {
				log_transfer_out(events_log_fd, get_vector_time(), local_id, order.s_amount, order.s_dst);
				*current_balance -= order.s_amount;
				
				Message forward_msg;
				advance_vector_time(TIME_OMITTED);
				copy_transfer_msg(&forward_msg, &msg_work);
				
				send(pipe_io, order.s_dst, &forward_msg);
			}
			else if (order.s_dst == local_id) {
				log_transfer_in(events_log_fd, get_vector_time(), local_id, order.s_amount, order.s_src);
				*current_balance += order.s_amount;
				
				Message msg_ack;
				advance_vector_time(TIME_OMITTED);
				init_ack_msg(&msg_ack);
				
				send(pipe_io, PARENT_ID, &msg_ack);
			}
			else {
				fputs("Incorrect transfer message", stderr);
				return -1;
			}
		}
		else if (STOP == msg_work.s_header.s_type && WORK == stage) {
			if (log_done(events_log_fd, get_vector_time(), local_id, *current_balance) < 0) {
				return -1;
			}
			
			done_count++;

			Message msg_done_send;
			advance_vector_time(TIME_OMITTED);
			if (init_done_msg(&msg_done_send, local_id, *current_balance) < 0) {
				fputs("init done message failed\n", stderr);
			}

			if (send_multicast(pipe_io, &msg_done_send) < 0) {
				fputs("send_multicast failed\n", stderr);
				return -1;
			}
			
			stage = END_WORK;
		}
		else if (DONE == msg_work.s_header.s_type) {
			done_count++;
			if (done_count == pipe_io->process_count - 1) {
				return EXIT_SUCCESS;
			}
		}
		else {
			fputs("Incorrect message\n", stderr);
			return -1;
		}
	}
}

int child_lifecycle(const pipe_io_ptr pipe_io, int8_t local_id, pid_t parent_pid, int events_log_fd, balance_t start_balance) {
	balance_t current_balance = start_balance;
	pid_t pid = getpid();	
    init_vector_time(local_id);
	
	if (log_started(events_log_fd, get_vector_time(), local_id, pid, parent_pid, current_balance) < 0) {
		return -1;
	}
	
	Message msg_started_send;
	advance_vector_time(TIME_OMITTED);
	if (init_started_msg(&msg_started_send, local_id, pid, parent_pid, current_balance) < 0) {
		fputs("init started message failed", stderr);
	}

	if (send_multicast(pipe_io, &msg_started_send) < 0) {
		fputs("send_multicast failed", stderr);
		return -1;
	}
	
	Message msg_started_receive;
	for (int8_t i = 1; i < pipe_io->process_count; i++) {
		if (i == local_id) {
			continue;
		}
		
		if (receive(pipe_io, i, &msg_started_receive) < 0) {
			return EXIT_FAILURE;
		}
		advance_vector_time(msg_started_receive.s_header.s_local_timevector);
	}
		
	if (log_recieved_all_started(events_log_fd, get_vector_time(), local_id) < 0) {
		return EXIT_FAILURE;
	}
	
	if (work_loop(pipe_io, events_log_fd, &current_balance, local_id) < 0) {
		return EXIT_FAILURE;
	}

	if (log_recieved_all_done(events_log_fd, get_vector_time(), local_id) < 0) {
		return EXIT_FAILURE;
	}

	if( close(events_log_fd) != 0) {
		return EXIT_FAILURE;
	}	
	
	if (close_pipe_fds(pipe_io->write_fd) < 0
		| close_pipe_fds(pipe_io->read_fd) < 0) {
		return EXIT_FAILURE;
	}
	return 0;
}
