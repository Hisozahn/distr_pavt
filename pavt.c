#include "banking.h"
#include "timer.h"
#include "definitions.h"

void init_transfer_order(TransferOrder *order, local_id src, local_id dst, balance_t amount) {
	order->s_src = src;
	order->s_dst = dst;
	order->s_amount = amount;
}

void transfer(void * parent_data, local_id src, local_id dst, balance_t amount) {
	TransferOrder order;
	init_transfer_order(&order, src, dst, amount);
	
	advance_lamport_time(TIME_OMITTED);
	Message msg_transfer;
	init_transfer_msg(&msg_transfer, &order);
	send(parent_data, src, &msg_transfer);
	
	Message msg_ack;
	receive(parent_data, dst, &msg_ack);
	advance_lamport_time(msg_ack.s_header.s_local_time);
}

void total_sum_snapshot()
{
    // TODO: Implement
}

int start_processes(pipe_io_ptr pipe_ios, int8_t child_count, balance_t *money) {
	pid_t child_pids[MAX_CHILD_COUNT] = {0};
	pid_t parent_pid = getpid();

	int events_log_fd = open(events_log, O_CREAT | O_APPEND | O_WRONLY, 0666);
	if (events_log_fd < 0) {
		return -1;
	}
	
	for (int8_t child_local_idx = 1; child_local_idx < child_count + 1; child_local_idx++) {
		pid_t cpid = fork();

		if (cpid < 0) {
			terminate_child_processes(child_pids);
			perror("fork()");
			return EXIT_FAILURE;
		}

		if (0 == cpid) {
			if (destroy_pipe_ios(pipe_ios, child_local_idx) < 0) {
				fputs("destroy failed", stderr);
				return EXIT_FAILURE;
			}
			return child_lifecycle(&pipe_ios[child_local_idx]
					, child_local_idx, parent_pid, events_log_fd, money[child_local_idx-1]);
		}
		else {
			child_pids[child_local_idx - 1] = cpid;
		}
	}
	

	if (destroy_pipe_ios(pipe_ios, PARENT_ID) < 0) {
		fputs("destroy failed", stderr);
		terminate_child_processes(child_pids);
		return EXIT_FAILURE;
	}
	return parent_lifecycle(&pipe_ios[PARENT_ID], child_pids, events_log_fd);
}

int main(int argc, char * argv[])
{
	int8_t child_count = get_child_count(argc, argv);
	if ( -1 == child_count) {
		return EXIT_FAILURE;
	}

	balance_t money[MAX_CHILD_COUNT] = {0};
	int opt_Idx = optind;
	
	for (int8_t child_Idx = 0; child_Idx < child_count; child_Idx++, opt_Idx++) {
		money[child_Idx] = atoi(argv[opt_Idx]);
	}
	
	pipe_io pipe_ios[MAX_PROCESS_COUNT] = { {0} };

	if (init_pipe_ios(pipe_ios, child_count + 1) < 0) {
		destroy_pipe_ios(pipe_ios, DESTROY_NOEXCEPT);
		return EXIT_FAILURE;
	}

	return start_processes(pipe_ios, child_count, money);
}
