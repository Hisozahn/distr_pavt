#ifndef __PIPE_MANAGER__H
#define __PIPE_MANAGER__H

#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "definitions.h"
#include "logger.h"

int init_pipe_ios(pipe_io_ptr pipe_ios, int8_t process_count);

int close_pipe_fds(const int *pipe_fds);

int destroy_pipe_ios(const pipe_io_ptr pipe_ios, int8_t except_local_id);

#endif // __PIPE_MANAGER__H
