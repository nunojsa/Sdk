include $(BUILD_ROOT)/make.detect

CUR_DIR=$(shell pwd)
SUBDIRS=$(ALL_SUBDIRS)

all: recursive_build

clean: recursive_clean

install: recursive_install

recursive_build:
	@for dir in $(SUBDIRS); do \
		if [ -d $$dir/ ]; then \
			if [ -f $$dir/Makefile ]; then \
				$(MAKE) -C $$dir/ ; \
			fi \
		fi \
	done

recursive_clean:
	@for dir in $(SUBDIRS); do \
		if [ -d $$dir/ ]; then \
			if [ -f $$dir/Makefile ]; then \
				$(MAKE) -C $$dir/ clean; \
			fi \
		fi \
	done

recursive_install:
	@for dir in $(SUBDIRS); do \
		if [ -d $$dir/ ]; then \
			if [ -f $$dir/Makefile ]; then \
				$(MAKE) -C $$dir/ install; \
			fi \
		fi \
	done


