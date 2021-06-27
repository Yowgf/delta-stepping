# delta-stepping project Makefile
# Main build rules
# ==============================================================================
# Everything in this configuration file has to do with the main (default build)
#   target.
# ------------------------------------------------------------------------------

# Links objects. Put first to avoid errors with .DEFAULT_GOAL assignment.
$(TARGET) : $(TARGET_DEPENDENCIES)
	$(info Linking main...)
	@$(LINK_CODE)
	$(info Done.)

# Compiles objects.
$(BUILD_MAIN)/%$(OBJECT_EXTENSION) :: $(APPLIANCE_MAIN)/%$(APP_EXTENSION) $(HEADER_MAIN)/%$(HEADER_EXTENSION)
	$(info $@)
	@$(COMPIL_OBJECT_CODE_MAIN)
