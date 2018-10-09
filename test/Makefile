LIBS:=$(OUT_DIR)/host/lib/libutils.a
BIN:=logger_test

LOCAL_OBJS:=logger_test.o
# -static prevents linking against shared library
LDFLAGS:=-static
INCLUDES:=-I$(BUILD_ROOT)/src/libs/utils

include $(BUILD_ROOT)/build/host_build.mk

# clean variables
include $(BUILD_ROOT)/build/clean_vars.mk

BIN:=dlink_list

LOCAL_OBJS:=dlink_list.o
INCLUDES:=-I$(BUILD_ROOT)/src/libs/utils -I$(BUILD_ROOT)/src/include

include $(BUILD_ROOT)/build/host_build.mk

# clean variables
include $(BUILD_ROOT)/build/clean_vars.mk

BIN:=timer_test
LIBS:=$(OUT_DIR)/host/lib/libutils.a

LOCAL_OBJS:=timer_test.o
LDFLAGS:=-static -lrt -lpthread
INCLUDES:=-I$(BUILD_ROOT)/src/libs/utils -I$(BUILD_ROOT)/src/include

include $(BUILD_ROOT)/build/host_build.mk

