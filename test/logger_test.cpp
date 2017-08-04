#include "stdlib.h"
#include "logger.h"
#include "stdio.h"

enum{
	LOG_TEST = 0,
	LOG_TEST_1,
	LOG_TEST_2,
	LOG_TEST_3
};

int main (void)
{ 
    struct logger_modularized test [] = {
	{LOG_TEST, "TEST"},
	{LOG_TEST_1, "TEST_1"},
	{LOG_TEST_2, "TEST_2"},
	{LOG_TEST_3,  "TEST_3"},
    };
    
    int test_3 = 5;
    int test_4 = 6;
    
    logger_init(test, sizeof(test)/sizeof(*test), LOG_LEV_INFO, LOG_DEST_STDOUT, NULL);
	
    LOG_FATAL("Print it: %d %d", test_3, test_4);
    LOG_DEBUG(LOG_TEST_1, "Debug on: %d %d", test_3, test_4);
    logger_set_log_level_by_module(LOG_TEST_1, LOG_LEV_DEBUG);
    LOG_DEBUG(LOG_TEST_1, "And now...debug on %d %d", test_3, test_4);
    return 1;
}
