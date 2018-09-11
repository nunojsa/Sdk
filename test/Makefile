LIBS:=-lutils
ELF:=logger_test
LINKLIBS:=$(OUT_DIR)lib/libutils.a 

#-static prevents linking against shared library
LDFLAGS:=-static -L$(OUT_DIR)/lib  
CFLAGS:=-I$(BUILD_ROOT)/src/libs/utils

LOCAL_OUT_DIR:=y
include $(BUILD_ROOT)/defaults.mk
include $(BUILD_ROOT)/build.mk


