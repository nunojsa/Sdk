APPS_DIR:=$(BUILD_ROOT)/src/apps
LIBS_DIR:=$(BUILD_ROOT)/src/libs
TEST_DIR:=$(BUILD_ROOT)/test
RED:='\033[0;31m'
WHITE:='\033[1;37m'
NC:='\033[0m'

ifeq ($(strip $(BUILD_ROOT)),)
message:=$(shell echo -e $(RED)Make sure you run "source scripts/env-setup.sh"$(NC))
$(error $(message))
endif

ifeq ($(strip $(SCRIPTS_DIR)),)
message:=$(shell echo -e $(RED)Make sure you run "source scripts/env-setup.sh"$(NC))
$(error $(message))
endif

ifeq ($(strip $(TARGET_BUILD)),)
message:=$(shell echo -e $(WHITE)No TARGET_BUILD selected! Default to host target)
$(warning $(message))
message:=$(shell echo -e Run "add_target_build [TARGET_BUILD]"$(NC))
$(info $(message))
GCC=$(shell hash gcc 2>/dev/null || echo "N")
ifeq ($(strip $(GCC)),N)
message:=$(shell echo -e $(RED)GCC not found! Is it installed?!$(NC))
$(error $(message))
endif
export TARGET_BUILD:=HOST
export AR:=ar
export AS:=as
export LD:="ld --verbose"
export CC:=gcc
export CXX:=g++
export CPP:=cpp
export NM:=nm
export STRIP:=strip
export SSTRIP:=sstrip
export OBJCOPY:=objcopy
export OBJDUMP:=objdump
export RANLIB:=ranlib
#setup install dirs
export OUT_DIR:=${BUILD_ROOT}/out/
endif

#global output directories
export OUT_OBJ_DIR:=$(OUT_DIR)obj
export OUT_BIN_DIR:=$(OUT_DIR)bin
export OUT_LIB_DIR:=$(OUT_DIR)lib
export LIBS_DIR
export APPS_DIR
#setup install directories
ifneq ($(strip $(ROOT_DIR)),)	
ifeq ($(strip $(INSTALL_BIN_DIR)),)
export INSTALL_BIN_DIR:=$(ROOT_DIR)usr/bin
endif
ifeq ($(strip $(INSTALL_LIB_DIR)),)
export INSTALL_LIB_DIR:=$(ROOT_DIR)usr/lib
endif
endif

all: src test

#So that make doesn't get confused and always tries to compile this directories. Important since i have directories with the same name as the recipes
.PHONY:  src test install


apps: libs
	@$(MAKE) -C $(APPS_DIR) all

libs:
	@$(MAKE) -C $(LIBS_DIR) all	

src: apps

test:
	@$(MAKE) -C $(TEST_DIR)

clean_libs:
	@$(MAKE) -C $(LIBS_DIR) clean

clean_apps:
	@$(MAKE) -C $(APPS_DIR) clean

clean_src: clean_libs clean_apps

clean_test:
	@$(MAKE) -C $(TEST_DIR) clean

clean: clean_src clean_test

clean_install_libs:
	@$(MAKE) -C $(LIBS_DIR) clean_install

clean_install_apps:
	@$(MAKE) -C $(APPS_DIR) clean_install

clean_install: clean_install_libs clean_install_apps 

distclean:
	@$(MAKE) -C $(LIBS_DIR) distclean
	@$(MAKE) -C $(APPS_DIR) distclean
	@$(MAKE) -C $(TEST_DIR) distclean
	@rm -rf $(OUT_DIR)

install_apps:
	@if [ -d "$(INSTALL_BIN_DIR)" ]; then \
		$(MAKE) -C $(APPS_DIR) install; \
	else \
		echo -e $(WHITE)[ERROR]: INSTALL_BIN_DIR does not exist. Run help_me on your shell to see how to properly add it$(NC); \
	fi

install_libs:
	@if [ -d "$(INSTALL_LIB_DIR)" ]; then \
		$(MAKE) -C $(LIBS_DIR) install; \
	else \
		echo -e $(WHITE)[ERROR]: INSTALL_LIB_DIR does not exist. Run help_me on your shell to see how to properly add it$(NC); \
	fi

install: install_libs install_apps

help:
	@echo "No help for you yet!!"
