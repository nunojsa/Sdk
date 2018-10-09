#ifndef TIMER_IMPL_H
#define TIMER_IMPL_H

#include "common.h"

typedef enum {
	TIMER_ONESHOT,
	TIMER_PERIODIC,
	TIMER_MAX,
} timer_mode;

struct timer {
	/*timer name*/
	char *id;
	/*callback function*/
	void (*func)(void *priv_data);
	/*user private data*/
	void *priv_data;
	/*must not be touched*/
	void *__priv;
};

#ifdef __cplusplus
extern "C" {
#endif
/*
 * @param new_timer Struct timer
 * @param mode Mode. Either TIMER_ONESHOT or TIMER_PERIODIC
 *
 * @return int 0 on success, otherwise a standard Linux error
 *         defined in errno-base.
 *
 * @brief Create a new timer for operation. The timer callback and
 *	  @mode must be properly defined otherwise -EINVAL is returned.
 */
int timer_new(struct timer *new_timer, const timer_mode mode);
/*
 * @param timer Struct timer
 *
 * @return int 0 on success, otherwise a standard Linux error
 *             defined in errno-base.
 *
 * @brief Destroy the given timer
 * @note @timer cannot be NULL
 */
int timer_destroy(const struct timer *timer);
/*
 * @param timer Struct timer
 *
 * @return int 0 on success, otherwise a standard Linux error
 *             defined in errno-base.
 *
 * @brief Checks if the given timer is counting.
 * @note @timer cannot be NULL
 */
int timer_pending(const struct timer *timer);
/*
 * @param timer Struct timer
 *
 * @return u64 Number of miliseconds for the timer to expire.
 *             Standard Linux error defined in errno-base otherwise.
 *
 * @brief Get the number of miliseconds for the timer to expire.
 * @note @timer cannot be NULL
 */
u64 timer_gettime_ms(const struct timer *timer);
/*
 * @param timer Struct timer
 * @param expire Time for expiration
 *
 * @return int 0 on success, otherwise a standard Linux error
 *             defined in errno-base.
 *
 * @brief Start the given timer with @expire time
 * @note @timer cannot be NULL
 */
int timer_fire(const struct timer *timer, u64 expire);
/*
 * @param timer Struct timer
 *
 * @return int 0 on success, otherwise a standard Linux error
 *             defined in errno-base.
 *
 * @brief Stop the given timer
 * @note @timer cannot be NULL
 */
int timer_stop(const struct timer *timer);

#ifdef __cplusplus
}
#endif
#endif
