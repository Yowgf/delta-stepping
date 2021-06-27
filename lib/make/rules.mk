# delta-stepping project Makefile
# Rules definition Makefile
# ==============================================================

RULES_DIR := $(AUX_MAKE_DIR)/rules

F_RULES_MAIN := $(RULES_DIR)/rules-main.mk
F_RULES_UNIT := $(RULES_DIR)/rules-unit.mk
F_RULES_PERF := $(RULES_DIR)/rules-perf.mk
F_RULES_GENERAL := $(RULES_DIR)/rules-general.mk

include $(F_RULES_MAIN)
include $(F_RULES_UNIT)
include $(F_RULES_PERF)
include $(F_RULES_GENERAL)

