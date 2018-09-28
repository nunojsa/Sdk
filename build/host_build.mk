include $(BUILD_ROOT)/build/common.mk

# Make sure gcc is present. For now we just support gcc
# This only makes sense for host builds, since the compiler for the
# target is checked as soon as the target build setup is done with add_target_build
$(call check_host_utility,gcc)

HOST_BUILD:=y
# build it
include $(BUILD_ROOT)/build/vars.mk
include $(BUILD_ROOT)/build/build.mk
