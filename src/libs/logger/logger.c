#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "logger.h"
#include <stdarg.h>

#define MAX_BUFFER_LEN		512
#define MAX_FILE_SIZE		1024*1024*10	//10MB
#define FILE_DEFAUT_NAME	"logger.txt"

/*\todo 
 * Handle when LOG_DEST is to a file. Special case of wrapping max size
 * Handle close of the file and changes from LOG_DEST_FILE to others
 * Handle settigng log level per module
 * Possible to hide the logger function??
 * Try to force giving modularized names to the modules. Avoid to call macros with number but ultimately this is duty from the app calling the logger
 * Add timestamp
 * Handle errors with line size and wrong modules
 * Telnet handling
 * */
 

struct logger_modules{
	int curr_level;	
	struct logger_modularized modules; 
};

struct logger_struct{
	FILE *file;
	int file_size;
	int curr_level;
	int log_dest;
	unsigned char modules_cnt;
	struct logger_modules modules[MODULE_MAX_NUMBER];
}logger_ctl;

const char *levelStr [] = {"[FATAL]", "[ERROR]", "[WARN]", "[INFO]", "[DEBUG]", "[TRACE]"};


static unsigned char logger_modules_init(struct logger_modularized *logger_modules, const int modules_cnt, const int level, const int dest, const char *path)
{
	int cnt = 0;
	
	for(; cnt < modules_cnt && cnt < MODULE_MAX_NUMBER; cnt++)
	{
		logger_ctl.modules[cnt].curr_level = level;
		memcpy(&logger_ctl.modules[cnt].modules, &logger_modules[cnt], sizeof(struct logger_modularized));
	}	

	return cnt;
}

static unsigned char logger_get_module_idx(const int module)
{
	int cnt = 0;
		
	for(; cnt < logger_ctl.modules_cnt; cnt++)
	{
		if(logger_ctl.modules[cnt].modules.module == module)
			break;
	}
	
	if(cnt == logger_ctl.modules_cnt)
		cnt = MODULE_MAX_NUMBER;
	
	return cnt;	
}

void logger_close_file()
{
	if( logger_ctl.file != NULL && logger_ctl.log_dest == LOG_DEST_FILE)
		fclose(logger_ctl.file);
	
}

void logger_init(struct logger_modularized *logger_modules, const int modules_cnt, const int level, const int dest, const char *path)
{
	memset(&logger_ctl, 0, sizeof(logger_ctl));
	
	logger_ctl.curr_level 	= level;
	logger_ctl.log_dest	= dest;	
		
	if(dest == LOG_DEST_FILE){
		logger_ctl.file = fopen( path != NULL ? path : FILE_DEFAUT_NAME, "w+");
	}
	else if(dest == LOG_DEST_TELNET){
		/**\todo */
	}
	else
		logger_ctl.file = stderr;
	
	if(logger_modules != NULL && modules_cnt > 0)
		logger_ctl.modules_cnt = logger_modules_init(logger_modules, modules_cnt, level, dest, path);
}

logger_exit_code logger_set_log_level(const int level)
{
	if( (level<LOG_LEV_FATAL) || (level>LOG_LEV_TRACE) )
		return LOGGER_UNKNOWN_LEVEL;
	
	logger_ctl.curr_level = level;
	
	return LOGGER_SUCCESS;
}

logger_exit_code logger_set_log_level_by_module(const int module, const int level)
{
	unsigned char idx = logger_get_module_idx(module);

	if( (level<LOG_LEV_FATAL) || (level>LOG_LEV_TRACE) )
		return LOGGER_UNKNOWN_LEVEL;
	else if( idx >= MODULE_MAX_NUMBER )
		return LOGGER_UNKNOWN_MODULE;
		
	logger_ctl.modules[idx].curr_level = level;	

	return LOGGER_SUCCESS;
}

logger_exit_code logger_set_log_dest(const int dest, const char *path)
{
	if(dest < LOG_DEST_STDOUT || dest > LOG_DEST_FILE)
		return LOGGER_UNKNOWN_DEST;
		
   	if(dest != logger_ctl.log_dest){
		logger_ctl.log_dest = dest;
		
		if(dest == LOG_DEST_FILE){
			logger_ctl.file = fopen( path != NULL ? path : FILE_DEFAUT_NAME, "w+");
		}
		else if(dest == LOG_DEST_TELNET){
			/**\todo */
		}
		else
			logger_ctl.file = stderr;
	}
	
	return LOGGER_SUCCESS;		
}

void logger (const int level, const char *file, const int line, void *arg, ...)
{
	va_list ap;
	char buffer[MAX_BUFFER_LEN] = {0};
	int len = 0;
	long int module = (long int)arg;
	char *fmt = NULL;
	
	va_start(ap, arg);
	
	fmt = (module < MODULE_MAX_NUMBER) ? va_arg(ap, char *) : (char *)arg; 
		
	if((level<LOG_LEV_FATAL) || (level>LOG_LEV_TRACE)){
		printf("logger: Invalid log level (%u)\n", level);
		return;
	}
	
	if(logger_ctl.modules_cnt > 0 && module < MODULE_MAX_NUMBER){
		
		unsigned char idx = logger_get_module_idx(module);
		
		if( (idx < MODULE_MAX_NUMBER) && (len < MAX_BUFFER_LEN) )
		{
			if( logger_ctl.modules[idx].curr_level < level)
				return;
					
			len = snprintf(&buffer[len], MAX_BUFFER_LEN-len, "%s, %d...%s %s: ", file, line, logger_ctl.modules[idx].modules.name, levelStr[level]);
		}
	}
	else
	{
		if( level > logger_ctl.curr_level)
			return;
		
		len = snprintf(buffer, MAX_BUFFER_LEN, "%s, %d... %s: ", file, line, levelStr[level]);
	}

	if(len < MAX_BUFFER_LEN)
		vsnprintf(&buffer[len], MAX_BUFFER_LEN-len, fmt, ap);
	
	if( logger_ctl.log_dest == LOG_DEST_FILE ){
		logger_ctl.file_size += len;
		if( logger_ctl.file_size > MAX_BUFFER_LEN-len){
			/**\todo handle wraparound*/
		}
	}
	
	fprintf(logger_ctl.file, "%s\n", buffer);
	
	va_end(ap);
}
