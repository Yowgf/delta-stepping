# delta-stepping project Makefile
# General variable expansions
# ==============================================================================
# Things included here are aren't specific to a single build, but rather touch
#   on all of them.
# ------------------------------------------------------------------------------

# Compiled files directory list
OBJECT_DIRS := $(BUILD_MAIN) $(BUILD_UNIT)

# For use in creating directories rule
#-- Do not use "/" in the end!
#-- Otherwise it won't recognize the directory's existance.
OBJECT_MOD_DIRS  := $(foreach mod, $(MODULES), $(foreach obj_dir, $(OBJECT_DIRS), $(patsubst %, %/$(mod), $(obj_dir))))
