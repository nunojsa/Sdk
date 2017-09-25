LIBS:=-llog
ELF:=logger_test
LINKLIBS:=$(OUT_DIR)lib/liblog.a 

#-static prevents linking against shared library
LDFLAGS:=-static -L$(OUT_DIR)/lib  
CFLAGS:=-I$(BUILD_ROOT)/src/libs/logger

LOCAL_OUT_DIR:=y
include $(BUILD_ROOT)/make.defaults
include $(BUILD_ROOT)/make.build


