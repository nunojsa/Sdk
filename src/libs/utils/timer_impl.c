/* Timer implementation
 *
 * Simple module providing timer functionality. Two types of timers
 * can be defined. ONESHOT timer which will run only once (the user can
 * still re-arm it of the callback function). PERIODIC timer will re-run
 * automatically after the timer callback function.
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

struct timer_private {
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
	struct timer_private *priv = NULL;
	struct itimerspec value;

	assert(timer != NULL);

	priv = (struct timer_private *)timer->__priv;
	if (unlikely(!priv)) {
		return -ENODEV;
	}

	memset(&value, 0, sizeof(struct itimerspec));

	if (timer_settime(priv->timerid, 0, &value, NULL) < 0) {
		return -errno;
	}

	return 0;
}

u64 timer_gettime_ms(const struct timer *timer)
{
	struct timer_private *priv = NULL;
	struct itimerspec value;

	assert(timer != NULL);

	priv = (struct timer_private *)timer->__priv;
	if (unlikely(!priv)) {
		return -ENODEV;
	}

	if (timer_gettime(priv->timerid, &value) < 0) {
		return -errno;
	}

	timer_trace("Time to expire, s:%lu, ns:%lu\n", value.it_value.tv_sec,
						value.it_value.tv_nsec);

	return ((value.it_value.tv_sec * MS_IN_SECONDS) +
		 (value.it_value.tv_nsec / NS_IN_MSECONDS));
}

int timer_fire(const struct timer *timer, u64 expire)
{
	struct timer_private *priv = NULL;
	struct itimerspec value = { {0, 0}, {0, 0} };

	assert(timer != NULL);

	priv = (struct timer_private *)timer->__priv;
	if (unlikely(!priv)) {
		return -ENODEV;
	}

	value.it_value.tv_sec = expire/MS_IN_SECONDS;
	value.it_value.tv_nsec = (expire % MS_IN_SECONDS) * NS_IN_MSECONDS;

	if (priv->mode == TIMER_PERIODIC) {
		value.it_interval.tv_sec = expire/MS_IN_SECONDS;
		value.it_interval.tv_nsec = (expire % MS_IN_SECONDS) * NS_IN_MSECONDS;
	}

	if (timer_settime(priv->timerid, 0, &value, NULL) < 0) {
		return -errno;
	}

	return 0;
}

int timer_pending(const struct timer *timer)
{
	struct timer_private *priv = NULL;
	struct itimerspec value = { {0, 0}, {0, 0} };

	assert(timer != NULL);

	priv = (struct timer_private *)timer->__priv;
	if (unlikely(!priv)) {
		return -ENODEV;
	}

	if (timer_gettime(priv->timerid, &value) < 0) {
		return -errno;
	}

	return (value.it_value.tv_sec != 0 || value.it_value.tv_nsec != 0);
}

int timer_destroy(const struct timer *timer)
{
	struct timer_private *priv = NULL;
	int ret = 0;

	assert(timer != NULL);

	priv = (struct timer_private *)timer->__priv;
	if (unlikely(!priv)) {
		return -ENODEV;
	}

	ret = timer_delete(priv->timerid);
	if (ret < 0) {
		return -errno;
	}

	free(priv);
	return ret;
}

int timer_new(struct timer *new_timer, const timer_mode mode)
{
	int ret = -1;
	struct timer_private *priv = NULL;
	struct sigevent event;

	memset(&event, 0, sizeof(event));

	if (!new_timer || !new_timer->func || mode >= TIMER_MAX) {
		timer_debug("Invalid arguments...\n");
		return -EINVAL;
	}

	priv = (struct timer_private *)malloc(sizeof(struct timer_private));
	if (!priv) {
		timer_debug("Failed to allocate memory...\n");
		return -ENOMEM;
	}
	/*setup event*/
	event.sigev_notify = SIGEV_THREAD;
	event.sigev_notify_function = timer_internal_cb;
	event.sigev_value.sival_ptr = (void *)new_timer;

	/*create the timer*/
	if (timer_create(CLOCK_MONOTONIC, &event, &priv->timerid) < 0) {
		timer_debug("Failed to create the timer...\n");
		ret = -errno;
		goto fail;
	}
	/*set mode*/
	priv->mode = mode;
	/*set private data*/
	new_timer->__priv = priv;
	return 0;
fail:
	free(priv);
	return ret;
}
