# common functions and checks
RED:='\033[0;31m'
WHITE:='\033[1;37m'
NC:='\033[0m'

ifeq ($(strip $(TARGET_BUILD)),)
message:=$(shell echo -e $(RED)No TARGET_BUILD selected! RUN add_target_build [TARGET_BUILD]$(NC))
$(error $(message))
endif

# Check if the out directory exists. Can be removed with a distclean...
ifeq ($(shell test -d $(OUT_DIR) || echo "N"),N)
message:=$(shell echo -e $(WHITE)Out directories don\'t exist! Creating them...$(NC))
$(info $(message))
$(shell mkdir -p $(OUT_DIR)/host/obj)
$(shell mkdir -p $(OUT_DIR)/host/bin)
$(shell mkdir -p $(OUT_DIR)/host/lib)
$(shell mkdir -p $(OUT_DIR)/$(TARGET_BUILD)/lib)
$(shell mkdir -p $(OUT_DIR)/$(TARGET_BUILD)/lib)
$(shell mkdir -p $(OUT_DIR)/$(TARGET_BUILD)/lib)
endif

# used to check if some utility (eg: gcc) is present on
# the host system. It assumes that the utility is on the PATH env variable...
define check_host_utility
$(eval util:=$(shell hash $(1) 2>/dev/null || echo "N"))
$(eval message:=$(shell echo -e $(RED)$(1) not found in the PATH! Is it installed?!$(NC)))
$(if $(util),$(error $(message)),)
endef
