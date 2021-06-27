# delta-stepping project Makefile
# Perf build rules
# ==============================================================================
# Everything in this configuration file has to do with the perfTest build.
# ------------------------------------------------------------------------------

# Tests the program against set of entries and expected outputs
# Then measures times to local csv file
perfTest :: $(TARGET)
	@bash $(TEST_SCRIPT) $(TEST_STEM) $(TEST_IN_EXTENSION) $(TEST_OUT_EXTENSION) $(TEST_NUMBER)
