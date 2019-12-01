#include "ipc_ext.h"

void init_msg_header(MessageHeader *msg_header, uint16_t payload_len, int16_t type, timevector_t  local_time) {
	msg_header->s_magic = MESSAGE_MAGIC;
	msg_header->s_payload_len = payload_len;
	msg_header->s_type = type;
    for (int i = 0; i < MAX_PROCESS_ID; i++)
	    msg_header->s_local_timevector[i] = local_time.vec[i];
}

int init_started_msg(Message *msg, int8_t local_id, pid_t pid, pid_t parent_pid, balance_t balance) {
	ssize_t payload_length = snprintf(msg->s_payload, MAX_PAYLOAD_LEN
			, log_started_fmt, get_vector_time(), local_id, pid, parent_pid, balance);
	
	if (payload_length < 0) {
		return -1;
	}
	init_msg_header(&msg->s_header, payload_length, STARTED, get_vector_timevec());
	return 0;
}

int init_done_msg(Message *msg, int8_t local_id, balance_t balance) {
	ssize_t payload_length = snprintf(msg->s_payload, MAX_PAYLOAD_LEN, log_done_fmt, get_vector_time(), local_id, balance);
	
	if (payload_length < 0) {
		return -1;
	}
	
	init_msg_header(&msg->s_header, payload_length, DONE, get_vector_timevec());
	
	return 0;
}

void init_transfer_msg(Message *msg, const TransferOrder *order) {
	init_msg_header(&msg->s_header, sizeof(*order), TRANSFER, get_vector_timevec());
	
	memmove(msg->s_payload, order, sizeof(*order));
}

void copy_transfer_msg(Message *dst, const Message *src) {
    timevector_t v = get_vector_timevec();

	memmove(dst, src, sizeof(*dst));
    for (int i = 0; i < MAX_PROCESS_ID; i++)
	    dst->s_header.s_local_timevector[i] = v.vec[i];
	
}

void init_stop_msg(Message *msg) {
	init_msg_header(&msg->s_header, 0, STOP, get_vector_timevec());
}

void init_ack_msg(Message *msg) {
	init_msg_header(&msg->s_header, 0, ACK, get_vector_timevec());
}

void init_balance_msg(Message *msg, balance_t balance, timestamp_t *vec_time)
{
    BalanceState state = {0};

    init_msg_header(&msg->s_header, sizeof(BalanceState), BALANCE_STATE, get_vector_timevec());
    state.s_balance = balance;
    for (int i = 0; i < MAX_PROCESS_ID; i++)
        state.s_timevector[i] = vec_time[i];

	memmove(msg->s_payload, &state, sizeof(state));
}

void init_vtime_msg(Message *msg, timestamp_t vec_time)
{
    init_msg_header(&msg->s_header, sizeof(vec_time), SNAPSHOT_VTIME, get_vector_timevec());
    memmove(msg->s_payload, &vec_time, sizeof(vec_time));
}

void init_snap_ack_msg(Message *msg)
{
	init_msg_header(&msg->s_header, 0, SNAPSHOT_ACK, get_vector_timevec());
}

void init_empty_msg(Message *msg)
{
	init_msg_header(&msg->s_header, 0, EMPTY, get_vector_timevec());
}
