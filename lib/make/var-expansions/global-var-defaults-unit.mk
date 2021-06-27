# delta-stepping project Makefile
# Unit variable defaults
# ==============================================================================
# Everything in this configuration file is related to the main (default) build.
# ------------------------------------------------------------------------------

GLOBAL_VAR_LIST_UNIT := SOURCE_UNIT HEADER_UNIT APPLIANCE_UNIT BUILD_UNIT UNIT_MAIN_FILE UNIT_TARGET

DEFAULT_SOURCE_UNIT = $(SOURCE)/unit

DEFAULT_HEADER_UNIT    = $(SOURCE_UNIT)/header
DEFAULT_APPLIANCE_UNIT = $(SOURCE_UNIT)/appliance

DEFAULT_BUILD_UNIT = $(BUILD)/unit

DEFAULT_UNIT_MAIN_FILE = $(APPLIANCE_UNIT)/unit_main.cpp
DEFAULT_UNIT_TARGET    = $(BUILD)/unit_main.exe
