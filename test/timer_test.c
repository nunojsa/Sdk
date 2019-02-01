#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#include "logger.h"
#include "timer_impl.h"

static int stop = FALSE;
static void timer_1_cb(void *priv);
static void timer_2_cb(void *priv);

struct timer *oneshot = NULL;
struct timer *periodic = NULL;

int n = 0;

static void timer_1_cb(void *priv)
{
	struct timer *oneshot = (struct timer *)priv;
	static int i;

	if (i == 3) {
		log_info("Exiting timer_1.\n");
		stop = TRUE;
	} else {
		u64 ms = timer_gettime_ms(periodic);

		log_info("Here...\n");
		log_debug("Re-arming timer \"%s\". Periodic time: %ld...\n",
				timer_get_name(oneshot), ms);
		if (i == 2) {
			log_debug("Stoping periodic timer...\n");
			if (timer_stop(periodic) < 0) {
				log_error("Failed to stop periodic timer...\n");
			}
		}
		timer_fire(oneshot, 2 * MS_IN_SECONDS);
		i++;
	}
}

static void timer_2_cb(void *priv)
{
	int *here = (int *)priv;

	log_debug("Timer \"%s\" expired, cnt %d\n", timer_get_name(periodic), (*here)++);
}

int main(void)
{
	logger_init(NULL, 0, LOG_LEV_DEBUG);

	log_info("Create oneshot\n");
	oneshot = timer_new("TIMER_ONESHOT", TIMER_ONESHOT, timer_1_cb, NULL);
	if (!oneshot) {
		log_error("Failed to create the timer\n");
		return -1;
	}

	log_info("Created oneshot\n");
	/*should not be printed*/
	if (timer_pending(oneshot) > 0) {
		log_info("Oneshot is not pending...\n");
	}

	/*3 seconds*/
	if (timer_fire(oneshot, 3 * MS_IN_SECONDS)) {
		log_error("Failed to start the timer\n");
		return -1;
	}

	periodic = timer_new("TIMER_PERIODIC", TIMER_PERIODIC, timer_2_cb, &n);
	if (!periodic) {
		log_error("Failed to create the timer\n");
		return -1;
	}

	/*should be printed*/
	if (timer_pending(oneshot) > 0) {
		log_info("Oneshot is now pending...\n");
	}

	/*4 seconds periodic*/
	if (timer_fire(periodic, 1 * MS_IN_SECONDS)) {
		log_error("Failed to start the timer\n");
		return -1;
	}

	while (!stop) {
		sleep(1);
	}

	if (timer_destroy(oneshot) < 0 ||
	    timer_destroy(periodic)) {
		log_error("Failed to destroy one of the timers...\n");
	}

	return 0;
}
