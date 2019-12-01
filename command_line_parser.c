#include "command_line_parser.h"

int8_t get_child_count(int argc, char *argv[]) {
	
	int opt;
	long child_count = 0;
	char *end = NULL;

	/*extern int opterr*/ opterr = 0;

	while ((opt = getopt(argc, argv, "p:")) != -1) {
			if ('p' == opt) {
					child_count = strtol(optarg, &end, 10);
					break;
			}
	}
	
	int option_p_argument_is_valid = (0 == errno)
		&& (NULL != end && '\0' == *end)
		&& (0 < child_count && child_count <= MAX_CHILD_COUNT)
		&& child_count == argc - get_money_first_index();
	
	if (option_p_argument_is_valid) {
			return (int8_t)child_count;
	}
	else {
			return -1;
	}
}

int8_t get_money_first_index() {
	return /* extern int */ optind;
}
