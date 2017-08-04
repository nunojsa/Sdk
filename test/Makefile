LIBS:=-llog
ELF:=logger_test

#-static prevents linking against shared library
LDFLAGS:=-static -L$(OUT_DIR)/lib -L$(LIBS_DIR)/logger/out/lib 
CFLAGS:=-I$(BUILD_ROOT)/src/libs/logger

LOCAL_OUT_DIR:=y
include $(BUILD_ROOT)/make.defaults
include $(BUILD_ROOT)/make.build


