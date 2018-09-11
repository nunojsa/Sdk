_CC=echo	"	CC	$(notdir $@)"
_CXX=echo       "	CXX	$(notdir $@)"
_LD=echo        "	LD	$(notdir $@)"
_CLEAN=echo     "	CLEAN	`echo $$obj | sed -e 's/.*\///'`"
_CLEAN_BIN=echo "	CLEAN	$(notdir $(1))"
_AR=echo        "	AR	$(notdir $@)"
_INSTALL=echo   "	INSTALL	$(notdir $(shell pwd))"
_UNINSTALL=echo "	UNINSTALL $(notdir $(shell pwd))"
_DISTCLEAN=echo "	RM	$(OUT_DIR)"

#default dirs.
ifneq ($(strip $(LOCAL_OUT_DIR)),)
	OUT_DIR:=$(shell pwd)/out/
	OUT_OBJ_DIR:=$(OUT_DIR)obj
ifeq ($(strip $(ELF)),)
	OUT_LIB_DIR:=$(OUT_DIR)lib
else
	OUT_BIN_DIR:=$(OUT_DIR)bin
endif
endif

#Check if this path is defined. If not, we are probably just compiling a single module and it was not
#exported by the main Makefile.
ifeq ($(strip $(OUT_OBJ_DIR:)),)
	OUT_OBJ_DIR:=$(OUT_DIR)obj
	OUT_LIB_DIR:=$(OUT_DIR)lib
	OUT_BIN_DIR:=$(OUT_DIR)bin
endif

ifneq ($(strip $(ELF)),)
ELF:=$(addprefix $(OUT_BIN_DIR)/,$(ELF))
endif

ifneq ($(strip $(LIB_STATIC)),)
LIB_STATIC:=$(addprefix $(OUT_LIB_DIR)/,$(LIB_STATIC))
endif

ifneq ($(strip $(LIB_DYNAMIC)),)
LIB_DYNAMIC:=$(addprefix $(OUT_LIB_DIR)/,$(LIB_DYNAMIC))
endif

ifeq ($(strip $(LOCAL_OBJS)),)
	OBJS:=$(addprefix $(OUT_OBJ_DIR)/,$(patsubst %.c,%.o,$(wildcard *.c)))
	OBJS+=$(addprefix $(OUT_OBJ_DIR)/,$(patsubst %.cpp,%.o,$(wildcard *.cpp)))
endif

ifeq ($(strip $(LOCAL_CFLAGS)),)
	CFLAGS+=-O2 -Werror -Wall -Wextra
endif
