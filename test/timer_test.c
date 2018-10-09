#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#include "logger.h"
#include "timer_impl.h"

static int stop = FALSE;
static void timer_1_cb(void *priv);
static void timer_2_cb(void *priv);

struct timer oneshot = {
	.id = "TIMER_ONESHOT",
	.func = timer_1_cb,
	.priv_data = &oneshot,
};

struct timer periodic = {
	.id = "TIMER_PERIODIC",
	.func = timer_2_cb,
	.priv_data = &periodic,
};

static void timer_1_cb(void *priv)
{
	struct timer *oneshot = (struct timer *)priv;
	static int i;

	if (i == 3) {
		log_info("Exiting timer_1.\n");
		stop = TRUE;
	} else {
		u64 ms = timer_gettime_ms(&periodic);

		log_debug("Re-arming timer \"%s\". Periodic time: %ld...\n",
				oneshot->id, ms);
		if (i == 2) {
			log_debug("Stoping periodic timer...\n");
			if (timer_stop(&periodic) < 0) {
				log_error("Failed to stop periodic timer...\n");
			}
		}
		timer_fire(oneshot, 2 * MS_IN_SECONDS);
		i++;
	}
}

static void timer_2_cb(void *priv)
{
	struct timer *periodic = (struct timer *)priv;

	log_debug("Timer \"%s\" expired\n", periodic->id);
}

int main(void)
{
	int ret = 0;

	logger_init(NULL, 0, LOG_LEV_DEBUG);

	ret = timer_new(&oneshot, TIMER_ONESHOT);
	if (ret < 0) {
		log_error("Failed to create the timer\n");
		return -1;
	}

	/*should not be printed*/
	if (timer_pending(&oneshot) > 0) {
		log_info("Oneshot is not pending...\n");
	}

	/*3 seconds*/
	if (timer_fire(&oneshot, 3 * MS_IN_SECONDS)) {
		log_error("Failed to start the timer\n");
		return -1;
	}

	ret = timer_new(&periodic, TIMER_PERIODIC);
	if (ret < 0) {
		log_error("Failed to create the timer\n");
		return -1;
	}

	/*should be printed*/
	if (timer_pending(&oneshot) > 0) {
		log_info("Oneshot is now pending...\n");
	}

	/*4 seconds periodic*/
	if (timer_fire(&periodic, 3 * MS_IN_SECONDS)) {
		log_error("Failed to start the timer\n");
		return -1;
	}

	while (!stop) {
		sleep(1);
	}

	if (timer_destroy(&oneshot) < 0 ||
	    timer_destroy(&periodic)) {
		log_error("Failed to destroy one of the timers...\n");
	}

	return 0;
}
