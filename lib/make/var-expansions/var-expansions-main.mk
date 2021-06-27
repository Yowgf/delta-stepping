# delta-stepping project Makefile
# Main variable expansions
# ==============================================================================
# Everything in this configuration file is related to the main (default) build.
# ------------------------------------------------------------------------------

# Header files list
HEADER_MAIN_FILES := $(wildcard $(patsubst %, $(HEADER_MAIN)/%/*.$(HEADER_EXTENSION), $(MODULES)))

# Appliance files list
APPLIANCE_MAIN_FILES := $(wildcard $(patsubst %, $(APPLIANCE_MAIN)/%/*.$(APP_EXTENSION), $(MODULES)))

# Searches for files with ~$(OBJECT_EXTENSION)~ extension
OBJECT_MAIN_FILES := $(patsubst $(APPLIANCE_MAIN)%$(APP_EXTENSION), $(BUILD_MAIN)%$(OBJECT_EXTENSION), $(APPLIANCE_MAIN_FILES))

# Compilation code for each object file
COMPIL_OBJECT_CODE_MAIN = $(CXX) $(FLAGS) -I $(HEADER_MAIN) -c $< -o $@

# Linking code
LINK_CODE = $(CXX) $(FLAGS) -I $(HEADER_MAIN) $(OBJECT_MAIN_FILES) $(MAIN_FILE) -o $@

BUILD_MODS_MAIN := $(patsubst %, $(BUILD_MAIN)/%, $(MODULES))

TARGET_DEPENDENCIES := $(MAIN_FILE) $(HEADER_MAIN_FILES) $(APPLIANCE_MAIN_FILES) $(BUILD_MODS_MAIN) $(OBJECT_MAIN_FILES)
