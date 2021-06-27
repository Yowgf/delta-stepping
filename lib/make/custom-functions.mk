# delta-stepping project Makefile
# Global variables custom definitions.
# ==============================================================

CUSTOM_FUNCTIONS_DIR := $(AUX_MAKE_DIR)/custom-functions

F_ERROR_FUNCTIONS            := $(CUSTOM_FUNCTIONS_DIR)/error-functions.mk
F_INSPECTION_FUNCTIONS       := $(CUSTOM_FUNCTIONS_DIR)/inspection-functions.mk
F_LARGE_PROCESSING_FUNCTIONS := $(CUSTOM_FUNCTIONS_DIR)/large-processing-functions.mk

include $(F_ERROR_FUNCTIONS)
include $(F_INSPECTION_FUNCTIONS)
include $(F_LARGE_PROCESSING_FUNCTIONS)

