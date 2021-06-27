# delta-stepping project Makefile
# Variable expansions Makefile
# ==============================================================

VAR_EXPANSIONS_DIR := $(AUX_MAKE_DIR)/var-expansions

F_VAR_EXPANSIONS_MAIN := $(VAR_EXPANSIONS_DIR)/var-expansions-main.mk
F_VAR_EXPANSIONS_UNIT := $(VAR_EXPANSIONS_DIR)/var-expansions-unit.mk
F_VAR_EXPANSIONS_GENERAL := $(VAR_EXPANSIONS_DIR)/var-expansions-general.mk

F_GLOBAL_VAR_DEFAULTS_MAIN := $(VAR_EXPANSIONS_DIR)/global-var-defaults-main.mk
F_GLOBAL_VAR_DEFAULTS_UNIT := $(VAR_EXPANSIONS_DIR)/global-var-defaults-unit.mk
F_GLOBAL_VAR_DEFAULTS_GENERAL := $(VAR_EXPANSIONS_DIR)/global-var-defaults-general.mk

# Set defaults of some variables (format DEFAULT_<VAR_NAME>)
# The order MAIN -> UNIT -> GENERAL is important here
include $(F_GLOBAL_VAR_DEFAULTS_MAIN)
include $(F_GLOBAL_VAR_DEFAULTS_UNIT)
include $(F_GLOBAL_VAR_DEFAULTS_GENERAL) # Has to be included last

# This call only affects those variables that have a default
#   value defined.
$(call lp_apply_defaults, $(GLOBAL_VAR_LIST))

# Checks definition of global variables.
# Function from inspection-functions file ~i_check_vars~ used here.
GLOBAL_VAR_STATUS :=  $(shell $(call i_check_vars, $(GLOBAL_VAR_LIST)))
temp_status := $(lastword $(GLOBAL_VAR_STATUS))
ifneq "$(temp_status)" "0"
  $(call e_fatal, $(temp_status) undefined global variables: \
    $(wordlist 1, $(temp_status), $(GLOBAL_VAR_STATUS)). \
    Verify file $(F_GLOBAL_VARS))
endif
undefine temp_status

# Import extensive and more complex variable definitions
include $(F_VAR_EXPANSIONS_MAIN)
include $(F_VAR_EXPANSIONS_UNIT)
include $(F_VAR_EXPANSIONS_GENERAL)

