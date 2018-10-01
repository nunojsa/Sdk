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
$(eval $(call do_x_install,install,$(BIN),$(INSTALL_BIN_DIR)))
$(eval $(call do_x_install,uninstall,$(notdir $(BIN)),$(INSTALL_LIB_DIR)))
install: install_$(BIN)
uninstall: uninstall_$(notdir $(BIN))
else ifneq ($(strip $(LIB_STATIC)),)
$(eval $(call do_x_install,install,$(LIB_STATIC),$(INSTALL_LIB_DIR)))
$(eval $(call do_x_install,uninstall,$(notdir $(LIB_STATIC)),$(INSTALL_LIB_DIR)))
install: install_$(LIB_STATIC)
uninstall: uninstall_$(notdir $(LIB_STATIC))
else ifneq ($(strip $(LIB_DYNAMIC)),)
$(eval $(call do_x_install,install,$(LIB_DYNAMIC),$(INSTALL_LIB_DIR)))
$(eval $(call do_x_install,uninstall,$(notdir $(LIB_DYNAMIC)),$(INSTALL_LIB_DIR)))
install: install_$(LIB_DYNAMIC)
uninstall: uninstall_$(notdir $(LIB_DYNAMIC))
endif