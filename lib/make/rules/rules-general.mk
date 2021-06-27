# delta-stepping project Makefile
# General rules
# ==============================================================================
# Things included here are aren't specific to a single build, but rather touch
#   on all of them.
# ------------------------------------------------------------------------------

.PHONY : clean unitTest perfTest

# The --parents switch here allows to automatically create parent directories when needed.
$(OBJECT_MOD_DIRS) ::
	@-mkdir --parents $@

# Deletes all the directories supposed to contain objects.
clean ::
ifneq "$(origin OBJECT_DIRS)" "file"
		$(info Variable "OBJECT_DIRS" used for "clean" left undefined.)
		$(info Please check the file in $(GLOBAL_VARS))
else
		$(eval OBJECT_DIRS := $(wildcard $(OBJECT_DIRS) $(TARGET)))
ifneq ($(strip $(OBJECT_DIRS)),)
		$(info Deleting all the directories and subfolders:)
		$(info $(BUILD))
		@echo
		@echo Are you sure? \(Y/N\)
		@read choice && \
		if [[ "$$choice" = "y" ]] || [[ "$$choice" = "Y" ]]; then \
		{ ( $(foreach dir, $(BUILD), rm --verbose --preserve-root --recursive --force "$(dir)"; ) ) && \
			echo Cleaning successful.; } || \
			$(call e_prev_cmd, Cleaning of some directory failed.) \
		else \
		echo No cleaning was performed.; \
		fi
endif
endif

# Sink for unidentified patterns.
% ::
	@echo Archive or Pattern "$@" not found!
