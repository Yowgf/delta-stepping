# delta-stepping project Makefile
# Unit build rules
# ==============================================================================
# Everything in this configuration file has to do with the unit build.
# ------------------------------------------------------------------------------

$(BUILD_UNIT)/%$(OBJECT_EXTENSION) :: $(APPLIANCE_UNIT)/%$(APP_EXTENSION) $(HEADER_UNIT)/%$(HEADER_EXTENSION)
	$(info $@)
	@$(COMPIL_OBJECT_CODE_UNIT)

# Performs the various unit tests available
unitTest :: $(UNIT_TARGET_DEPENDENCIES)
	@echo Linking unit...
	@$(UNIT_LINK_CODE)
	@echo Done.
	@echo Running unit tests...
	@./$(UNIT_TARGET)
	@echo Unit tests all done.
