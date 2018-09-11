#ifndef	LOGGER_H
#define LOGGER_H
#include <ctype.h>

enum {
	LOG_LEV_FATAL,
	LOG_LEV_ERROR,
	LOG_LEV_WARN,
	LOG_LEV_INFO,
	LOG_LEV_DEBUG,
	LOG_LEV_TRACE,
	LOG_LEV_MAX
};

enum {
	LOG_DEST_STDOUT,
	LOG_DEST_FILE,
	LOG_DEST_MAX
};

#define MODULE_MAX_NUMBER	10
#define MODULE_UNDEF		-1

struct logger_modularized {
	int module;
	const char *name;
};

#define LOG(level, module, fmt, ...) \
	logger(level, __FILE__, __LINE__, module, fmt, ##__VA_ARGS__)

/*normal macros*/
#define log_fatal(fmt, ...) \
	LOG(LOG_LEV_FATAL, MODULE_UNDEF, fmt, ##__VA_ARGS__)
#define log_error(fmt, ...) \
	LOG(LOG_LEV_ERROR, MODULE_UNDEF, fmt, ##__VA_ARGS__)
#define log_warn(fmt, ...) \
	LOG(LOG_LEV_WARN,  MODULE_UNDEF, fmt, ##__VA_ARGS__)
#define log_info(fmt, ...) \
	LOG(LOG_LEV_INFO,  MODULE_UNDEF, fmt, ##__VA_ARGS__)
#define log_debug(fmt, ...) \
	LOG(LOG_LEV_DEBUG, MODULE_UNDEF, fmt, ##__VA_ARGS__)
#define log_trace(fmt, ...) \
	LOG(LOG_LEV_TRACE, MODULE_UNDEF, fmt, ##__VA_ARGS__)

/*use the _m macros to specify log by module*/
#define log_fatal_m(module, fmt, ...) \
	LOG(LOG_LEV_FATAL, module, fmt, ##__VA_ARGS__)
#define log_error_m(module, fmt, ...) \
	LOG(LOG_LEV_ERROR, module, fmt, ##__VA_ARGS__)
#define log_warn_m(module, fmt, ...) \
	LOG(LOG_LEV_WARN, module, fmt, ##__VA_ARGS__)
#define log_info_m(module, fmt, ...) \
	LOG(LOG_LEV_INFO, module, fmt, ##__VA_ARGS__)
#define log_debug_m(module, fmt, ...) \
	LOG(LOG_LEV_DEBUG, module, fmt, ##__VA_ARGS__)
#define log_trace_m(module, fmt, ...) \
	LOG(LOG_LEV_TRACE, module, fmt, ##__VA_ARGS__)

#ifdef __cplusplus
extern "C" {
#endif

/*
 * @brief print all the current configurations.
 */
void logger_print_all(void);
/*
 * @param level loglevel to set
 *
 * @brief Set the global log level
 */
void logger_set_log_level(const int level);
/*
 * @param module module id
 * @param level log level to set
 *
 * @brief Set the log level of a specific module
 */
void logger_set_log_level_by_module(const int module, const int level);
/*
 * @param dest log destination
 * @param path file path
 *
 * @brief change the log destinations. Either a file or standard output.
 */
void logger_set_log_dest(const int dest, const char *path);

/*
 * @param level log level
 * @param file file of the message
 * @param line line of the message
 * @param module module id
 * @param fmt printf style string
 *
 * @brief logger function. Should not be called directly.
 *        Use the helper macros instead.
 */
void logger(const int level, const char *file, const int line,
	    int module, const char *fmt, ...);
/*
 * @param logger_modules supported modules for the logger
 * @param modules_cnt number of modules
 * @param level initial global log level
 * @param dest log destination
 * @param path file path
 *
 * @brief  init the logger. Destinations defaults to the console.
 *         The user has to explicitly call logger_set_log_dest.
 */
void logger_init(struct logger_modularized *logger_modules,
		 const int modules_cnt, const int level);

/*
 * @brief Close all allocated resources
 */
void logger_close(void);

#ifdef __cplusplus
}
#endif
#endif //logger.h





