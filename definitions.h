#ifndef __DEFINITIONS__H
#define __DEFINITIONS__H

#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

enum {
	MAX_CHILD_COUNT = 9,
	MAX_PROCESS_COUNT = MAX_CHILD_COUNT + 1,
	MAX_PIPE_COUNT = 90
};

static const int PROHIBITED_FD = -1;
static const int8_t DESTROY_NOEXCEPT = -1;

typedef struct pipe_io* pipe_io_ptr;

typedef struct pipe_io {
	int8_t process_count;
	int write_fd[MAX_PROCESS_COUNT];
	int read_fd[MAX_PROCESS_COUNT];
} pipe_io;

#include "timer.h"
#include "ipc_ext.h"
#include "banking.h"
#include "child_process.h"
#include "command_line_parser.h"
#include "common.h"
#include "definitions.h"
#include "ipc_ext.h"
#include "ipc.h"
#include "logger.h"
#include "pa2345.h"
#include "parent_process.h"
#include "pavt.h"
#include "pipe_manager.h"
#include "timer.h"

//#define ENABLE_DEBUG_PRINT


#endif // __DEFINITIONS__H
