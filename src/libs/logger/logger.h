#ifndef	LOGGER_H
#define LOGGER_H
#include <ctype.h>

#define LOG_LEV_FATAL		0
#define LOG_LEV_ERROR		1
#define LOG_LEV_WARN		2
#define LOG_LEV_INFO		3
#define LOG_LEV_DEBUG		4
#define LOG_LEV_TRACE		5

#define LOG_DEST_STDOUT		0
#define LOG_DEST_TELNET		1
#define LOG_DEST_FILE		2

#define MODULE_MAX_NUMBER	10

typedef enum
{
	LOGGER_SUCCESS,
	LOGGER_UNKNOWN_LEVEL,
	LOGGER_UNKNOWN_MODULE,
	LOGGER_UNKNOWN_DEST,
	LOGGER_ERROR,
	
}logger_exit_code;


struct logger_modularized
{
    int module;
    const char *name;
};

#define LOG(level, args1, ...)		logger(level, __FILE__, __LINE__, (void *)args1, ##__VA_ARGS__)
 
       
#define LOG_FATAL(...)			LOG(LOG_LEV_FATAL, __VA_ARGS__)
#define LOG_ERROR(...)			LOG(LOG_LEV_ERROR, __VA_ARGS__)
#define LOG_WARN(...)			LOG(LOG_LEV_WARN,  __VA_ARGS__)
#define LOG_INFO(...)			LOG(LOG_LEV_INFO,  __VA_ARGS__)
#define LOG_DEBUG(...)			LOG(LOG_LEV_DEBUG, __VA_ARGS__)
#define LOG_TRACE(...)			LOG(LOG_LEV_TRACE, __VA_ARGS__)

#ifdef __cplusplus
extern "C" {
#endif
logger_exit_code logger_set_log_level(const int level);

logger_exit_code logger_set_log_level_by_module(const int module, const int level);

logger_exit_code logger_set_log_dest(const int dest, const char *path);

void logger (const int level, const char *file, const int line, void *arg, ...);

void logger_init(struct logger_modularized *logger_modules, const int modules_cnt, const int level, const int dest, const char *path);
#ifdef __cplusplus
}
#endif
#endif //logger.h





