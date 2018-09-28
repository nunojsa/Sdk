LIBS:=$(OUT_DIR)/host/lib/libutils.a
BIN:=logger_test

LOCAL_OBJS:=logger_test.o
# -static prevents linking against shared library
LDFLAGS:=-static
INCLUDES:=-I$(BUILD_ROOT)/src/libs/utils

include $(BUILD_ROOT)/build/host_build.mk

