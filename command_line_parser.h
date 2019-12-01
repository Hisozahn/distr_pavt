#ifndef __COMMAND_LINE_PARSER__H
#define __COMMAND_LINE_PARSER__H

#include <getopt.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <stdio.h>

#include "definitions.h"

int8_t get_child_count(int argc, char *argv[]);

int8_t get_money_first_index();

#endif // __COMMAND_LINE_PARSER__H
