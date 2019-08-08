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
$(if $(shell PATH=$(PATH) which $(1)),,\
$(error $(shell echo -e $(RED)$(1) not found in the PATH! Is it installed?!$(NC))))
endef

# It handles .so symlinks. It follows the linux way, the link name
# (libname without any version info after .so) symlinks against
# soname and soname against the real lib. Also soname is libname.so.major
# (1): The directory where the links should be created
# (2): The soname
# (3): The link lib symlink
define do_ld_symlink
@cd $(1) 1>/dev/null; \
ln -sf $(2) $(3); \
ldconfig -nN .; \
cd - 1>/dev/null;
endef

# It handles the removal of .so symlinks
# (1): The directory where the links are
# (2): Soname symlink
# (3): Link lib symlink
define do_clean_ld_symlink
@rm $(1)/$(2) 2>/dev/null || true
@rm $(1)/$(3) 2>/dev/null || true
endef
