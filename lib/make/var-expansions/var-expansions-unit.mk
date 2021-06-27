# delta-stepping project Makefile
# Unit variable expansions
# ==============================================================================
# Everything in this configuration file is related to the unitTest build.
# ------------------------------------------------------------------------------

# Header files list
HEADER_UNIT_FILES    := $(wildcard $(patsubst %, $(HEADER_UNIT)/%/*.$(HEADER_EXTENSION), $(MODULES)))

# Appliance files list
APPLIANCE_UNIT_FILES := $(wildcard $(patsubst %, $(APPLIANCE_UNIT)/%/*.$(APP_EXTENSION), $(MODULES)))

# Searches for files with ~$(OBJECT_EXTENSION)~ extension
OBJECT_UNIT_FILES := $(patsubst $(APPLIANCE_UNIT)%$(APP_EXTENSION), $(BUILD_UNIT)%$(OBJECT_EXTENSION), $(APPLIANCE_UNIT_FILES))

# Compilation code for each object file
COMPIL_OBJECT_CODE_UNIT = $(CXX) $(FLAGS) -I $(HEADER_MAIN) -I $(HEADER_UNIT) -c $< -o $@

# Linking code
UNIT_LINK_CODE = $(CXX) $(FLAGS) -I $(HEADER_MAIN) -I $(HEADER_UNIT) $(OBJECT_MAIN_FILES) $(OBJECT_UNIT_FILES) $(UNIT_MAIN_FILE) -o $(UNIT_TARGET)

BUILD_MODS_UNIT := $(patsubst %, $(BUILD_UNIT)/%, $(MODULES))

UNIT_TARGET_DEPENDENCIES = $(UNIT_MAIN_FILE) $(HEADER_UNIT_FILES) $(APPLIANCE_UNIT_FILES) $(HEADER_MAIN_FILES) $(APPLIANCE_MAIN_FILES) $(OBJECT_MOD_DIRS) $(OBJECT_MAIN_FILES) $(OBJECT_UNIT_FILES)
