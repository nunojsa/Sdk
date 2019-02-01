/* Timer implementation
 *
 * Simple module providing timer functionality. Two types of timers
 * can be defined. ONESHOT timer which will run only once (the user can
 * still re-arm it of the callback function). PERIODIC timer will re-run
 * automatically after the timer callback function.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 */
#include <signal.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

#include "timer_impl.h"
#ifdef CONFIG_TIMER_DEBUG
#include "logger.h"
#endif
#include "compile.h"

#ifdef CONFIG_TIMER_DEBUG
#define timer_debug(fmt, arg...)	log_debug(fmt, ##arg)
#define timer_trace(fmt, arg...)	log_trace(fmt, ##arg)
#else
#define timer_debug(fmt, arg...)
#define timer_trace(fmt, arg...)
#endif

#define TIMER_NAME_MAX	256

struct timer {
	/*timer name*/
	char id[TIMER_NAME_MAX];
	/*callback function*/
	void (*func)(void *priv_data);
	/*user private data*/
	void *priv_data;
	/*timer id*/
	timer_t timerid;
	/*timer mode*/
	timer_mode mode;
};

static void timer_internal_cb(union sigval val)
{
	struct timer *timer = (struct timer *)val.sival_ptr;

	if (unlikely(!timer->func)) {
		timer_debug("Timer callback not defined...\n");
		return;
	}

	timer_trace("Timer \"%s\" expired...\n", timer->id ? timer->id :
						"Unknown");
	/*call user callback now*/
	timer->func(timer->priv_data);
}

int timer_stop(const struct timer *timer)
{
	struct itimerspec value;

	assert(timer != NULL);

	memset(&value, 0, sizeof(struct itimerspec));

	if (timer_settime(timer->timerid, 0, &value, NULL) < 0) {
		return -errno;
	}

	return 0;
}

u64 timer_gettime_ms(const struct timer *timer)
{
	struct itimerspec value;

	assert(timer != NULL);

	if (timer_gettime(timer->timerid, &value) < 0) {
		return -errno;
	}

	timer_trace("Time to expire, s:%lu, ns:%lu\n", value.it_value.tv_sec,
						value.it_value.tv_nsec);

	return ((value.it_value.tv_sec * MS_IN_SECONDS) +
		 (value.it_value.tv_nsec / NS_IN_MSECONDS));
}

int timer_fire(const struct timer *timer, u64 expire)
{
	struct itimerspec value = { {0, 0}, {0, 0} };

	assert(timer != NULL);

	value.it_value.tv_sec = expire/MS_IN_SECONDS;
	value.it_value.tv_nsec = (expire % MS_IN_SECONDS) * NS_IN_MSECONDS;

	if (timer->mode == TIMER_PERIODIC) {
		value.it_interval.tv_sec = expire/MS_IN_SECONDS;
		value.it_interval.tv_nsec = (expire % MS_IN_SECONDS) * NS_IN_MSECONDS;
	}

	if (timer_settime(timer->timerid, 0, &value, NULL) < 0) {
		return -errno;
	}

	return 0;
}

int timer_pending(const struct timer *timer)
{
	struct itimerspec value = { {0, 0}, {0, 0} };

	assert(timer != NULL);

	if (timer_gettime(timer->timerid, &value) < 0) {
		return -errno;
	}

	return (value.it_value.tv_sec != 0 || value.it_value.tv_nsec != 0);
}

int timer_destroy(struct timer *timer)
{
	int ret = 0;

	assert(timer != NULL);

	ret = timer_delete(timer->timerid);
	if (ret < 0) {
		return -errno;
	}

	free(timer);
	return ret;
}

inline timer_mode timer_get_mode(const struct timer *timer)
{
	assert(timer != NULL);

	return timer->mode;
}

inline const char *timer_get_name(const struct timer *timer)
{
	assert(timer != NULL);

	return timer->id;
}

struct timer *timer_new(const char *name, const timer_mode mode,
		void (*func)(void *priv_data), void *priv_data)
{
	struct timer *new = NULL;
	struct sigevent event;

	memset(&event, 0, sizeof(event));

	if (!func || mode >= TIMER_MAX) {
		timer_debug("Invalid arguments...\n");
		return NULL;
	}

	new = (struct timer *)malloc(sizeof(struct timer));
	if (!new) {
		timer_debug("Failed to allocate memory...\n");
		return NULL;
	}
	/*setup event*/
	event.sigev_notify = SIGEV_THREAD;
	event.sigev_notify_function = timer_internal_cb;
	event.sigev_value.sival_ptr = (void *)new;

	/*create the timer*/
	if (timer_create(CLOCK_MONOTONIC, &event, &new->timerid) < 0) {
		timer_debug("Failed to create the timer...\n");
		goto fail;
	}

	if (name != NULL) {
		strncpy(new->id, name, sizeof(new->id) - 1);
	}

	/*set cb*/
	new->func = func;
	/*set mode*/
	new->mode = mode;
	/*set private data*/
	new->priv_data = (priv_data != NULL) ? priv_data : new;

	return new;
fail:
	free(new);
	return NULL;
}
