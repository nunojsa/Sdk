#ifndef TIMER_IMPL_H
#define TIMER_IMPL_H

#include "common.h"

typedef enum {
	TIMER_ONESHOT,
	TIMER_PERIODIC,
	TIMER_MAX,
} timer_mode;

struct timer;

#ifdef __cplusplus
extern "C" {
#endif
/*
 * @param name Timer name
 * @param mode Either TIMER_ONESHOT or TIMER_PERIODIC
 * @param func callback function
 * @param priv_data private data
 *
 * @note if @priv_data is NULL, the newly created timer is passed
 * 	    as agrgument of @func.
 *
 * @return struct timer* NULL on error
 *
 * @brief Create a new timer for operation. The timer callback and
 *	  @mode must be properly defined otherwise NULL is returned.
 */
struct timer *timer_new(const char *name, const timer_mode mode,
			void (*func)(void *priv_data), void *priv_data);
/*
 * @param timer Struct timer
 *
 * @return int 0 on success, otherwise a standard Linux error
 *             defined in errno-base.
 *
 * @brief Destroy the given timer
 * @note @timer cannot be NULL
 */
int timer_destroy(struct timer *timer);
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
/*
 * @param timer  Struct timer
 * @return const char* Timer name
 */
const char *timer_get_name(const struct timer *timer);
/*
 * @param timer  Struct timer
 * @return timer_mode
 */
timer_mode timer_get_mode(const struct timer *timer);
#ifdef __cplusplus
}
#endif
#endif
