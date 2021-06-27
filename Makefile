# delta-stepping project Makefile
# Main Makefile
# ==============================================================================
#
# This make file bundletries to cover normal build "make", unit testing "make
#   unitTest", and performance tests "make perfTest".
# ------------------------------------------------------------------------------
#
# Options:
#
# unitTest ------------------- builds program and performs unit tests
# perfTest ------------------- builds program and performs performance testing
# clean ---------------------- cleaning built files rule
# ------------------------------------------------------------------------------

SHELL := /bin/bash

# Auxiliary makefiles addresses
ALL_FILES_VARS     := F_CUSTOM_FUNCTIONS F_GLOBAL_VARS F_VAR_EXPANSION \
                      F_RULES
AUX_MAKE_DIR       := lib/make
F_CUSTOM_FUNCTIONS := $(AUX_MAKE_DIR)/custom-functions.mk
F_GLOBAL_VARS      := $(AUX_MAKE_DIR)/global-var.mk
F_VAR_EXPANSION    := $(AUX_MAKE_DIR)/var-expansions.mk
F_RULES            := $(AUX_MAKE_DIR)/rules.mk

# This function is defined here due to its most essential character.
# The idea is not to use "include" unless you are certain all files
#   exist.
define i_check_files =
	for file_var in $(foreach file, $(1), $($(file))); do
		if [[ ! -e "$$file_var" ]]; then
			echo "$$file_var 1";
			exit 1;
		fi
	done;
	echo 0
endef

FILE_VAR_STATUS := $(shell $(call i_check_files, $(ALL_FILES_VARS)))
ifneq "$(lastword $(FILE_VAR_STATUS))" "0"
  $(error Essential file "$(word 1, $(FILE_VAR_STATUS))" not found.)
endif

# Informs global variables
include $(F_GLOBAL_VARS)

# Informs custom functions
include $(F_CUSTOM_FUNCTIONS)

# Expands variables
include $(F_VAR_EXPANSION)

# Informs rules
include $(F_RULES)
