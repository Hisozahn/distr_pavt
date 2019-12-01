#include "ipc_ext.h"

void init_msg_header(MessageHeader *msg_header, uint16_t payload_len, int16_t type, timestamp_t  local_time) {
	msg_header->s_magic = MESSAGE_MAGIC;
	msg_header->s_payload_len = payload_len;
	msg_header->s_type = type;
	msg_header->s_local_time = local_time;
}

int init_started_msg(Message *msg, int8_t local_id, pid_t pid, pid_t parent_pid, balance_t balance) {
	ssize_t payload_length = snprintf(msg->s_payload, MAX_PAYLOAD_LEN
			, log_started_fmt, get_lamport_time(), local_id, pid, parent_pid, balance);
	
	if (payload_length < 0) {
		return -1;
	}
	init_msg_header(&msg->s_header, payload_length, STARTED, get_lamport_time());
	return 0;
}

int init_done_msg(Message *msg, int8_t local_id, balance_t balance) {
	ssize_t payload_length = snprintf(msg->s_payload, MAX_PAYLOAD_LEN, log_done_fmt, get_lamport_time(), local_id, balance);
	
	if (payload_length < 0) {
		return -1;
	}
	
	init_msg_header(&msg->s_header, payload_length, DONE, get_lamport_time());
	
	return 0;
}

void init_transfer_msg(Message *msg, const TransferOrder *order) {
	init_msg_header(&msg->s_header, sizeof(*order), TRANSFER, get_lamport_time());
	
	memmove(msg->s_payload, order, sizeof(*order));
}

void copy_transfer_msg(Message *dst, const Message *src) {
	memmove(dst, src, sizeof(*dst));
	dst->s_header.s_local_time = get_lamport_time();
	
}

void init_stop_msg(Message *msg) {
	init_msg_header(&msg->s_header, 0, STOP, get_lamport_time());
}

void init_ack_msg(Message *msg) {
	init_msg_header(&msg->s_header, 0, ACK, get_lamport_time());
}
