# default target
all: $(BIN) $(LIB_STATIC) $(LIB_DYNAMIC)

# generic targets for clean and install
define do_clean
clean_$(1):
	@for obj in $(2); do \
		if [ -f $$$$obj ]; then \
			$(_CLEAN); \
			rm -f $$$$obj; \
		fi; \
	done
	@rm -f $(2:.o=.d)
	@if [ -f "$(1)" ]; then \
		$(_CLEAN_BIN); \
		rm -f $(1); \
	fi
endef

# define target specific variables so that, we can
# define more than one artifact in one Makefile
$(BIN): my_LDFLAGS:=$(LDFLAGS)
$(BIN): my_INCLUDES:=$(INCLUDES)
$(BIN): my_CFLAGS:=$(CFLAGS)
$(BIN): my_OBJS:=$(OBJS)
# generic rules.
$(BIN): $(OBJS) $(LIBS)
	@$(_LD)
	@$(CC) -o $@ $(my_OBJS) $(my_LDFLAGS)

$(LIB_STATIC): my_INCLUDES:=$(INCLUDES)
$(LIB_STATIC): my_CFLAGS:=$(CFLAGS)
$(LIB_STATIC): $(OBJS)
	@$(_AR)
	@$(AR) -rcs $@ $^

$(LIB_DYNAMIC): my_LDFLAGS:=$(LDFLAGS)
$(LIB_DYNAMIC): my_CFLAGS:=$(CFLAGS)
$(LIB_DYNAMIC): my_INCLUDES:=$(INCLUDES)
$(LIB_DYNAMIC): $(OBJS)
	@$(_LD)
	@$(CC) -shared -o $@ $^ $(my_LDFLAGS)

# define a generic clean target
ifneq ($(strip $(BIN)),)
$(eval $(call do_clean,$(BIN),$(OBJS)))
clean: clean_$(BIN)
else ifneq ($(strip $(LIB_STATIC)),)
$(eval $(call do_clean,$(LIB_STATIC),$(OBJS)))
clean: clean_$(LIB_STATIC)
else ifneq ($(strip $(LIB_DYNAMIC)),)
$(eval $(call do_clean,$(LIB_DYNAMIC),$(OBJS)))
clean: clean_$(LIB_DYNAMIC)
endif

# pattern rules
$(OUT_OBJ_DIR)/%.o: %.c
	@$(_CC)
	@$(CC) $(my_INCLUDES) $(my_CFLAGS) -MD -MP -c $< -o $@

$(OUT_OBJ_DIR)/%.o: %.cpp
	@$(_CXX)
	@$(CXX) $(my_INCLUDES) $(my_CFLAGS) -MD -MP -c $< -o $@

$(OUT_OBJ_DIR)/%.o: %.cc
	@$(_CXX)
	@$(CXX) $(my_INCLUDES) $(my_CFLAGS) -MD -MP -c $< -o $@

# include dependencies targets
-include $(OBJS:.o=.d)
