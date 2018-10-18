_CC=echo	"	CC	$(notdir $@)"
_CXX=echo       "	CXX	$(notdir $@)"
_LD=echo        "	LD	$(notdir $@)"
_CLEAN=echo     "	CLEAN	`echo $$$$obj | sed -e 's/.*\///'`"
_CLEAN_BIN=echo "	CLEAN	$(notdir $(1))"
_AR=echo        "	AR	$(notdir $@)"
_INSTALL=echo   "	INSTALL	$(notdir $(2))"
_UNINSTALL=echo "	UNINSTALL $(notdir $(2))"

LDFLAGS_AUX=$(shell echo $(LDFLAGS) | grep "\-l")

ifeq ($(strip $(HOST_BUILD)),y)
	OUT_OBJ_DIR:=$(OUT_DIR)/host/obj
	OUT_BIN_DIR:=$(OUT_DIR)/host/bin
	OUT_LIB_DIR:=$(OUT_DIR)/host/lib
	# override CC variable
	CC:=gcc
	AR:=ar
	LD:="ld --verbose"
	CXX:=g++
	CPP:=cpp
	OUT:=host
else
	OUT_OBJ_DIR:=$(OUT_DIR)/$(TARGET_BUILD)/obj
	OUT_BIN_DIR:=$(OUT_DIR)/$(TARGET_BUILD)/bin
	OUT_LIB_DIR:=$(OUT_DIR)/$(TARGET_BUILD)/lib
	OUT:=$(TARGET_BUILD)
endif

ifneq ($(strip $(LDFLAGS_AUX)),)
LDFLAGS:=$(addprefix -L$(OUT_DIR)/$(OUT)/lib ,$(LDFLAGS))
LDFLAGS+=-Wl,-rpath='$$ORIGIN/:$$ORIGIN/../lib/'
endif

ifneq ($(strip $(BIN)),)
BIN:=$(addprefix $(OUT_BIN_DIR)/,$(BIN))
endif

ifneq ($(strip $(LIB_STATIC)),)
LIB_STATIC:=$(addprefix $(OUT_LIB_DIR)/,$(LIB_STATIC))
endif

ifneq ($(strip $(LIB_DYNAMIC)),)
LIB_DYNAMIC:=$(addprefix $(OUT_LIB_DIR)/,$(LIB_DYNAMIC))
CFLAGS:=-fPIC
endif

ifeq ($(strip $(LOCAL_OBJS)),)
	OBJS:=$(addprefix $(OUT_OBJ_DIR)/,$(patsubst %.c,%.o,$(wildcard *.c)))
	OBJS+=$(addprefix $(OUT_OBJ_DIR)/,$(patsubst %.cpp,%.o,$(wildcard *.cpp)))
	OBJS+=$(addprefix $(OUT_OBJ_DIR)/,$(patsubst %.cc,%.o,$(wildcard *.cc)))
else
	OBJS:=$(addprefix $(OUT_OBJ_DIR)/,$(LOCAL_OBJS))
endif

ifeq ($(strip $(LOCAL_CFLAGS)),)
	CFLAGS+=-O2 -Werror -Wall -Wextra
else
	CFLAGS=$(LOCAL_CFLAGS)
endif

