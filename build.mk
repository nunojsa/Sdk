#generic targets for clean and install
define do_clean
clean_bin:
	@if [ -f "$(1)" ]; then \
		$(_CLEAN_BIN); \
		rm -f $(1); \
	fi
endef

define do_x_install
$(1):
	@if [ "$(1)" == "install" ]; then \
		if [ ! -f "$(3)/$(2)" ]; then \
			$(_INSTALL); \
			install -m 775 $(4)/$(2) $(3); \
		fi \
	else \
		if [ -f "$(3)/$(2)" ]; then \
			$(_UNINSTALL); \
			rm -f $(3)/$(2); \
                fi \
	fi
endef

#generic rules.
$(ELF): $(OBJS) $(LINKLIBS)
	@$(_LD)
	@$(CC) -o $(ELF) $(OBJS) $(LDFLAGS) $(LIBS)

$(LIB_STATIC): $(OBJS)
	@$(_AR)
	@$(AR) -rcs $@ $(OBJS)

clean: clean_obj clean_bin

#the for loop is just for convenience, to see all the OBJS being removed
clean_obj:
	@for obj in $(OBJS); do \
		if [ -f $$obj ]; then \
			$(_CLEAN); \
			rm -f $$obj; \
		fi; \
	done
	@rm -f $(OBJS:.o=.d)

#define a generic clean target
ifneq ($(strip $(ELF)),)
$(eval $(call do_clean,$(ELF)))
$(eval $(call do_x_install,install,$(notdir $(ELF)),$(INSTALL_BIN_DIR),$(OUT_BIN_DIR)))
$(eval $(call do_x_install,clean_install,$(notdir $(ELF)),$(INSTALL_LIB_DIR)))
else ifneq ($(strip $(LIB_STATIC)),)
$(eval $(call do_clean,$(LIB_STATIC)))
$(eval $(call do_x_install,install,$(notdir $(LIB_STATIC)),$(INSTALL_LIB_DIR),$(OUT_LIB_DIR)))
$(eval $(call do_x_install,clean_install,$(notdir $(LIB_STATIC)),$(INSTALL_LIB_DIR)))
else ifneq ($(strip $(LIB_DYNAMIC)),)
$(eval $(call do_clean,$(LIB_DYNAMIC)))
$(eval $(call do_x_install,install,$(notdir $(LIB_DYNAMIC)),$(INSTALL_LIB_DIR),$(OUT_LIB_DIR)))
$(eval $(call do_x_install,clean_install,$(notdir $(LIB_DYNAMIC)),$(INSTALL_LIB_DIR)))
endif

distclean:
	@if [ -d "$(OUT_DIR)" ]; then \
		$(_DISTCLEAN); \
		rm -rf $(OUT_DIR); \
	fi

#pattern rules
$(OUT_OBJ_DIR)/%.o: %.c
	@$(_CC)
	@$(CC) $(CFLAGS) -c -o $@ $<

$(OUT_OBJ_DIR)/%.o: %.cpp
	@$(_CXX)
	@$(CXX) $(CFLAGS) -c -o $@ $<

#automatic prerequisites generation. Look to section 4.14 of GNU Make
#Using the -MM (dont include compiler headers) option which is supported by the GNU compiler. Might be an issue for other compilers...
$(OUT_OBJ_DIR)/%.d: %.c
	@set -e; rm -f $@; \
	$(CC) -MM $(CPPFLAGS) $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,$(OUT_OBJ_DIR)\/\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(OUT_OBJ_DIR)/%.d: %.cpp
	@set -e; rm -f $@; \
	$(CC) -MM $(CPPFLAGS) $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,$(OUT_OBJ_DIR)\/\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$


# "|" means that the rule are order-only rules, meaning that it is imposing an ordering to build the target (dirs must exist),
# but does not force the target to be updated if one of these rules are executed (which for dirs is handy since the timestamps are changing whenever a file 
# is added/removed or changed) 
$(OBJS): | $(OUT_BIN_DIR) $(OUT_LIB_DIR) $(OUT_OBJ_DIR) 

$(OUT_BIN_DIR):
	@mkdir -p $(OUT_BIN_DIR)

$(OUT_LIB_DIR):
	@mkdir -p $(OUT_LIB_DIR)

$(OUT_OBJ_DIR):
	@mkdir -p $(OUT_OBJ_DIR)

include $(OBJS:.o=.d)
