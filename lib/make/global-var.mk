# delta-stepping project Makefile
# Global variables custom definitions.
# ==============================================================

GLOBAL_VAR_DIR := $(AUX_MAKE_DIR)/global-var

F_GLOBAL_VAR_MAIN := $(GLOBAL_VAR_DIR)/global-var-main.mk
F_GLOBAL_VAR_UNIT := $(GLOBAL_VAR_DIR)/global-var-unit.mk
F_GLOBAL_VAR_PERF := $(GLOBAL_VAR_DIR)/global-var-perf.mk
F_GLOBAL_VAR_GENERAL := $(GLOBAL_VAR_DIR)/global-var-general.mk

# Include custom definitions of global variables
# Leaving these empty will apply the default value
# Check file var_expansions.mk for details

include $(F_GLOBAL_VAR_MAIN)
include $(F_GLOBAL_VAR_UNIT)
include $(F_GLOBAL_VAR_PERF)
include $(F_GLOBAL_VAR_GENERAL)
