RED:='\033[0;31m'
WHITE:='\033[1;37m'
NC:='\033[0m'

ifeq ($(strip $(BUILD_ROOT)),)
message:=$(shell echo -e $(RED)Make sure you run \"source scripts/env-setup.sh\"$(NC))
$(error $(message))
endif

include $(BUILD_ROOT)/build/common.mk

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

uninstall_libs:
	@$(MAKE) -C $(LIBS_DIR) uninstall

uninstall_apps:
	@$(MAKE) -C $(APPS_DIR) uninstall

uninstall: uninstall_libs uninstall_apps

distclean:
	@if [ -d "$(OUT_DIR)" ]; then \
		echo "       RM      $(OUT_DIR)"; \
		rm -rf $(OUT_DIR); \
	fi

install_apps:
	@if [ -d "$(ROOT_DIR)" ]; then \
		$(MAKE) -C $(APPS_DIR) install; \
	else \
		echo -e $(RED)[ERROR]: ROOT_DIR does not exist. Run help_me on your shell to see how to properly add it!$(NC); \
		false; \
	fi

install_libs:
	@if [ -d "$(ROOT_DIR)" ]; then \
		$(MAKE) -C $(LIBS_DIR) install; \
	else \
		echo -e $(RED)[ERROR]: ROOT_DIR does not exist. Run help_me on your shell to see how to properly add it!$(NC); \
		false; \
	fi

install: install_libs install_apps

help:
	@echo "No help for you yet!!"
