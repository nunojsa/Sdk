#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include "logger.h"

enum{
	LOG_TEST = 0,
	LOG_TEST_1,
	LOG_TEST_2,
	LOG_TEST_3
};

static int quit = 0;

void exit_loop(int signal)
{
	if (signal == SIGINT) {
		quit = 1;
	}
}

int main (void)
{
	struct logger_modularized test [] = {
		{LOG_TEST, "TEST"},
		{LOG_TEST_1, "TEST_1"},
		{LOG_TEST_2, "TEST_2"},
		{LOG_TEST_3, "TEMPERATURE"},
	};
	struct sigaction sig;
	char const *msg = "I just want to create a big string...and make the file grown!\n";
	int test_3 = 5;
	int test_4 = 6;

	logger_init(test, sizeof(test)/sizeof(*test),
		    LOG_LEV_INFO);

	log_fatal("Print it: %d %d\n", test_3, test_4);
	log_debug_m(LOG_TEST_1, "Debug on: %d %d\n", test_3, test_4);
	logger_set_log_level_by_module(LOG_TEST_1, LOG_LEV_DEBUG);
	log_debug_m(LOG_TEST_1, "And now...debug on %d %d\n", test_3, test_4);
	log_trace("Tracing...\n");
	logger_set_log_level(LOG_LEV_TRACE);
	log_trace("Tracing now...\n");
	logger_set_log_dest(LOG_DEST_FILE, "file.txt");
	logger_set_log_level(LOG_LEV_INFO);
	logger_print_all();
	/*setup CTRL-C*/
	sig.sa_handler = exit_loop;
	sigaction(SIGINT, &sig, NULL);

	while (!quit) {
		printf("write msg. CTRL-C to gracefully stop the loop\n");
		log_info(msg);
		sleep(1);
	}

	logger_close();

	return 0;
}
