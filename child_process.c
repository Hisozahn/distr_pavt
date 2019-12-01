#include "child_process.h"

typedef struct {
    local_id        s_id;
    uint8_t         s_history_len;
    BalanceState    s_history[MAX_T + 1]; ///< Must be used as a buffer, unused
                                          ///< part of array shouldn't be transfered
} __attribute__((packed)) BalanceHistory;

void get_order_from_msg(const Message *msg, TransferOrder *order) {
	memcpy(order, msg->s_payload, sizeof(*order));
}

void init_local_history(BalanceHistory *history, int8_t local_id) {
	memset(history, 0, sizeof(*history));
	history->s_id = local_id;
}

void add_history_entry(BalanceHistory *history, balance_t current_balance, balance_t pending) {
	timestamp_t time = get_vector_time();
	
	int last_entry_idx = history->s_history_len - 1;
	
	if (last_entry_idx == -1) {
		
		history->s_history_len++;
		history->s_history[0].s_balance = current_balance;
		history->s_history[0].s_time = time;
		history->s_history[0].s_balance_pending_in = pending;
	}
	else {
		
		balance_t last_balance = history->s_history[last_entry_idx].s_balance;
		timestamp_t last_time = history->s_history[last_entry_idx].s_time;
		balance_t last_pending = history->s_history[last_entry_idx].s_balance_pending_in;

		for (uint8_t i = last_time + 1; i < time; i++) {
			
			history->s_history[history->s_history_len].s_balance = last_balance;
			history->s_history[history->s_history_len].s_time = i;
			history->s_history_len++;
		}
		
		if (last_pending != 0) {
			history->s_history[last_entry_idx + 1].s_balance_pending_in = last_pending;
		}
		
		history->s_history[history->s_history_len].s_balance_pending_in = pending;
		history->s_history[history->s_history_len].s_balance = current_balance;
		history->s_history[history->s_history_len].s_time = time;
		history->s_history_len++;
	}
}

int work_loop( pipe_io_ptr pipe_io, int events_log_fd, BalanceHistory *local_history
				, balance_t *current_balance, int8_t local_id) {
	int8_t done_count = 0;
    timestamp_t snap_time = -1;
	work_stage stage = WORK;
    int from;
	while (1) {
		Message msg_work;
        from = receive_any(pipe_io, &msg_work);
        if (from == -2)
            continue;
        if (from < 0)
            return -1;
        if (SNAPSHOT_VTIME != msg_work.s_header.s_type)
        {
		    advance_vector_time(msg_work.s_header.s_local_timevector);
        }
		
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
				add_history_entry(local_history, *current_balance, order.s_amount);
			}
			else if (order.s_dst == local_id) {
				log_transfer_in(events_log_fd, get_vector_time(), local_id, order.s_amount, order.s_src);
				*current_balance += order.s_amount;
				
				Message msg_ack;
				advance_vector_time(TIME_OMITTED);
				init_ack_msg(&msg_ack);
				
				send(pipe_io, PARENT_ID, &msg_ack);
				add_history_entry(local_history, *current_balance, 0);
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
        else if (SNAPSHOT_VTIME == msg_work.s_header.s_type) {
            snap_time = *(timestamp_t *)msg_work.s_payload;

            Message snap_ack;
            init_snap_ack_msg(&snap_ack);
            
            send(pipe_io, from, &snap_ack);
        }
        else if (EMPTY == msg_work.s_header.s_type) {
            timevector_t t = get_vector_timevec();
            if (snap_time > -1 && t.vec[0] >= snap_time)
            {
                Message bal;
                timevector_t vec;
                advance_vector_time(TIME_OMITTED);

                vec = get_vector_timevec();

                init_balance_msg(&bal, *current_balance, vec.vec);
                send(pipe_io, from, &bal);
                snap_time = -1;
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

	BalanceHistory local_history;
	init_local_history(&local_history, local_id);
	add_history_entry(&local_history, current_balance, 0);
	
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
	
	if (work_loop(pipe_io, events_log_fd, &local_history, &current_balance, local_id) < 0) {
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
