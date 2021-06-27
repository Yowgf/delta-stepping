# delta-stepping project Makefile
# Main variable defaults
# ==============================================================================
# Everything in this configuration file is related to the main (default) build.
# ------------------------------------------------------------------------------

GLOBAL_VAR_LIST_MAIN := SOURCE_MAIN HEADER_MAIN APPLIANCE_MAIN BUILD_MAIN MAIN_FILE TARGET

DEFAULT_SOURCE_MAIN = $(SOURCE)/main

DEFAULT_HEADER_MAIN    = $(SOURCE_MAIN)/header
DEFAULT_APPLIANCE_MAIN = $(SOURCE_MAIN)/appliance

DEFAULT_BUILD_MAIN = $(BUILD)/main

DEFAULT_MAIN_FILE = $(APPLIANCE_MAIN)/main.$(APP_EXTENSION)
DEFAULT_TARGET    = $(BUILD)/main.$(EXECUTABLE_EXTENSION)
