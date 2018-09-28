# include all default targets for libraries and apps. It is included in libs and apps Makefile
# to avoid duplication of the targets since the Makefiles should be identical!
tmp_subdir_list := $(shell egrep -L ^Detect */.detect 2> /dev/null)
SUBDIRS := $(subst /.detect,,$(tmp_subdir_list))


define do_task
$(1):
	@for dir in $(SUBDIRS); do \
		if [ -d $$$$dir ]; then \
			$(MAKE) -C $$$$dir $(2); \
		fi \
	done
endef

$(eval $(call do_task,all))
$(eval $(call do_task,clean,clean))
$(eval $(call do_task,install,install))
$(eval $(call do_task,uninstall,uninstall))

