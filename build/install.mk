define do_x_install
$(1)_$(2):
	@if [ "$(1)" == "install" ]; then \
		if [ -f "$(2)" ]; then \
			$(_INSTALL); \
			install -m 775 $(2) $(3); \
		fi \
	else \
		if [ -f "$(3)/$(2)" ]; then \
			$(_UNINSTALL); \
			rm -f $(3)/$(2); \
                fi \
	fi
endef

ifneq ($(strip $(BIN)),)
$(eval $(call do_x_install,install,$(BIN),$(ROOT_DIR)usr/bin))
$(eval $(call do_x_install,uninstall,$(notdir $(BIN)),$(ROOT_DIR)usr/bin))
install: install_$(BIN)
uninstall: uninstall_$(notdir $(BIN))
else ifneq ($(strip $(LIB_STATIC)),)
$(eval $(call do_x_install,install,$(LIB_STATIC),$(ROOT_DIR)usr/lib))
$(eval $(call do_x_install,uninstall,$(notdir $(LIB_STATIC)),$(ROOT_DIR)usr/lib))
install: install_$(LIB_STATIC)
uninstall: uninstall_$(notdir $(LIB_STATIC))
else ifneq ($(strip $(LIB_DYNAMIC)),)
$(eval $(call do_x_install,install,$(LIB_DYNAMIC),$(ROOT_DIR)usr/lib))
$(eval $(call do_x_install,uninstall,$(notdir $(LIB_DYNAMIC)),$(ROOT_DIR)usr/lib))
install: my_LINK_LIB:=$(LIB_LINK)
install: my_SONAME:=$(SONAME)
install: install_$(LIB_DYNAMIC)
	$(call do_ld_symlink,$(ROOT_DIR)/usr/lib,$(my_SONAME),$(my_LINK_LIB))
uninstall: my_LINK_LIB:=$(LIB_LINK)
uninstall: my_SONAME:=$(SONAME)
uninstall: uninstall_$(notdir $(LIB_DYNAMIC))
	@# clean the symlinks if there
	$(call do_clean_ld_symlink,$(ROOT_DIR)/usr/lib,$(my_LINK_LIB),$(my_SONAME))
endif
