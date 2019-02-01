/*
 * Logger implementation.
 *
 * This logger is not meant to be thread safe. It is an apllication logger, not a
 * system one. For an application logger, looks extreme to me to have locks
 * inside, slowing down the process. There is only a lock when the destination is a file,
 * so that a log file is not lost.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <stdarg.h>
#include <stdint.h>
#include <pthread.h>
#include "logger.h"

#define MAX_BUFFER_LEN		512
/*50MB*/
#define MAX_FILE_SIZE		(1024*1024*50)

struct logger_modules {
	int curr_level;
	struct logger_modularized modules;
};

static struct logger_struct {
	char path[MAX_BUFFER_LEN];
	FILE *file;
	int file_size;
	int curr_level;
	int log_dest;
	unsigned char modules_cnt;
	struct logger_modules modules[MODULE_MAX_NUMBER];
} logger_ctl;

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

/*must be in accordance with the enum...*/
static const char *levelStr[] = {
	"[FATAL]",
	"[ERROR]",
	"[WARN]",
	"[INFO]",
	"[DEBUG]",
	"[TRACE]",
};

static const char *destStr[] = {
	"[STDOUT]",
	"[FILE]",
};

#define logger_err(fmt, arg...) \
	printf("%s, %d... [ERROR]: "fmt"", __func__, __LINE__, ##arg)

/*
 * @param logger_modules modules to be initialized
 * @param modules_cnt number of modules added
 * @param level initial log level
 *
 * @return uint8_t the number of initialized modules.
 *
 * @brief initialize the logger added modules
 */
static uint8_t logger_modules_init(struct logger_modularized *logger_modules,
				   const int modules_cnt, const int level)
{
	int cnt = 0;

	if (modules_cnt >= MODULE_MAX_NUMBER) {
		return 0;
	}
	for (; cnt < modules_cnt; cnt++) {
		logger_ctl.modules[cnt].curr_level = level;
		memcpy(&logger_ctl.modules[cnt].modules,
		       &logger_modules[cnt], sizeof(struct logger_modularized));
	}

	return cnt;
}
/*
 * @param module module id
 *
 * @return uint8_t the idx of the module
 *                 MODULE_MAX_NUMBER in case the module does not exist
 *
 * @brief get the index of the module given by module.
 */
static uint8_t logger_get_module_idx(const int module)
{
	int cnt = 0;

	for (; cnt < logger_ctl.modules_cnt; cnt++) {
		if (logger_ctl.modules[cnt].modules.module == module) {
			break;
		}
	}

	return cnt;
}
/*
 * @return int -1 on failure
 *
 * @brief  handles the case where a file is running big. In this case,
 *         a filename.old file is created and the filename starts froms
 *         the begining.
 */
static int logger_handle_file_max(void)
{
	int len = 0;
	int a_len = 4; //.old size
	char *new_path = NULL;

	len = strlen(logger_ctl.path);
	new_path = (char *)malloc(len + a_len + 1);
	if (new_path == NULL) {
		perror("Failed to allocate memory for the new filename");
		/*just loose the old log in this case...*/
		goto reopen;
	}

	/*set the filename*/
	memset(new_path, 0, len + a_len + 1);
	strcpy(new_path, logger_ctl.path);
	strcat(new_path, ".old");

	/*let's rename the file now*/
	if (rename(logger_ctl.path, new_path) < 0) {
		perror("Failed to rename the log file");
	}

	free(new_path);
	/*let's just reopen the file for writing.*/
reopen:
	logger_ctl.file = freopen(logger_ctl.path, "w", logger_ctl.file);
	if (logger_ctl.file == NULL) {
		logger_err("Failed to reopen the file. Setting console as dest\n");
		/*fallback to console logging...*/
		logger_ctl.log_dest = LOG_DEST_STDOUT;
		logger_ctl.file_size = 0;
		return -1;
	}

	return 0;
}

void logger_print_all(void)
{
	int m = 0;

	printf("---------------------------------\n");
	printf("| LOGGER		        |\n");
	printf("|-------------------------------|\n");
	printf("|   level: %-16s     |\n", levelStr[logger_ctl.curr_level]);
	printf("|   dest : %-16s     |\n", destStr[logger_ctl.log_dest]);
	printf("----------------------------------------------------------\n");
	printf("|        MODULE            |           LEVEL             |\n");
	printf("----------------------------------------------------------\n");
	for (m = 0; m < logger_ctl.modules_cnt; m++) {
		printf("|        %-16s  |           %-16s  |\n",
		       logger_ctl.modules[m].modules.name,
		       levelStr[logger_ctl.modules[m].curr_level]);
		if (m == logger_ctl.modules_cnt - 1) {
			printf("----------------------------------------------------------\n");
		}
	}
}

void logger_close(void)
{
	if (logger_ctl.file && fileno(logger_ctl.file) != -1) {
		fclose(logger_ctl.file);
	}

	pthread_mutex_destroy(&lock);
}

void logger_init(struct logger_modularized *logger_modules,
		 const int modules_cnt, const int level)
{
	memset(&logger_ctl, 0, sizeof(logger_ctl));

	logger_ctl.curr_level = level;
	logger_set_log_dest(LOG_DEST_STDOUT, NULL);

	if (logger_modules != NULL && modules_cnt > 0) {
		logger_ctl.modules_cnt = logger_modules_init(logger_modules,
						modules_cnt, level);
	}
}

void logger_set_log_level(const int level)
{
	if (level >= LOG_LEV_MAX) {
		logger_err("Invalid log level, %d\n", level);
		return;
	}

	logger_ctl.curr_level = level;
}

void logger_set_log_level_by_module(const int module, const int level)
{
	uint8_t idx = logger_get_module_idx(module);

	if (level >= LOG_LEV_MAX) {
		logger_err("Invalid log level, %d\n", level);
		return;
	} else if (idx >= logger_ctl.modules_cnt) {
		logger_err("Cannot retrieve module, %d\n", module);
		return;
	}

	logger_ctl.modules[idx].curr_level = level;
}

void logger_set_log_dest(const int dest, const char *path)
{
	if (dest >= LOG_DEST_MAX) {
		logger_err("Invalid destination, %d\n", dest);
		return;
	}

	if (dest != logger_ctl.log_dest) {
		if (dest == LOG_DEST_FILE) {
			int maxlen = 0;

			if (!path) {
				logger_err("File path not specified...\n");
				return;
			}

			maxlen = strnlen(path, MAX_BUFFER_LEN);
			if (maxlen == MAX_BUFFER_LEN) {
				logger_err("Filename to big or not null terminated!!\n");
				return;
			}
			/*If it's the same file, let's continue where we stop...*/
			if (!strcmp(path, logger_ctl.path)) {
				goto change_dest;
			}

			/*close previous streams (if opened)...*/
			if (logger_ctl.file && fileno(logger_ctl.file) != -1) {
				fclose(logger_ctl.file);
			}
			logger_ctl.file = fopen(path, "w");
			if (logger_ctl.file == NULL) {
				perror("Failed to open file...Fallbacking to console");
				return;
			}

			strcpy(logger_ctl.path, path);
			logger_ctl.file_size = 0;
		} else if (dest != LOG_DEST_STDOUT) {
			logger_err("Unhandled destination, %d\n", dest);
			return;
		}
change_dest:
		logger_ctl.log_dest = dest;
	}
}

void logger(const int level, const char *file, const int line,
	    int module, const char *fmt, ...)
{
	va_list ap;
	char buffer[MAX_BUFFER_LEN] = {0};
	int len = 0;
	FILE *stream = stdout;

	va_start(ap, fmt);

	if (level > LOG_LEV_MAX) {
		logger_err("Invalid log level (%u)\n", level);
		return;
	}

	if (logger_ctl.modules_cnt > 0 && module >= 0) {
		uint8_t idx = logger_get_module_idx(module);

		if (idx < logger_ctl.modules_cnt) {
			if (logger_ctl.modules[idx].curr_level < level) {
				return;
			}

			len = snprintf(&buffer[len], MAX_BUFFER_LEN-len,
				       "%s, %d...%s %s: ", file, line,
				       logger_ctl.modules[idx].modules.name,
				       levelStr[level]);
		}
	} else {
		if (level > logger_ctl.curr_level) {
			return;
		}

		len = snprintf(buffer, MAX_BUFFER_LEN, "%s, %d... %s: ",
			       file, line, levelStr[level]);
	}

	if (len < MAX_BUFFER_LEN) {
		vsnprintf(&buffer[len], MAX_BUFFER_LEN-len, fmt, ap);
	}

	if (logger_ctl.log_dest == LOG_DEST_FILE) {
		/* This lock here is to prevent losing a log file when
		 * logger_handle_file_max() is called from concurrent threads.
		 * The next condition is usefull because with it, we can keep
		 * this lock inside the LOG_DEST_FILE case. @logger_ctl.file can
		 * be NULL in the case 2 concurrent threads get inside this
		 * condition (logger_ctl.log_dest == LOG_DEST_FILE) and the
		 * first to grab the lock failed in the logger_handle_file_max()
		 * call which means we are left with NULL in @logger_ctl.file.
		 */
		pthread_mutex_lock(&lock);
		if (logger_ctl.file != NULL) {
			stream = logger_ctl.file;
			logger_ctl.file_size += len;
			if (logger_ctl.file_size >= MAX_FILE_SIZE) {
				if (logger_handle_file_max() < 0) {
					stream = stdout;
				} else {
					logger_ctl.file_size = len;
				}
			}
		}
		pthread_mutex_unlock(&lock);
	}

	fprintf(stream, "%s", buffer);
	fflush(stream);

	va_end(ap);
}
