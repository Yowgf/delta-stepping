# delta-stepping project Makefile
# General variable defaults
# ==============================================================================
# Things included here are aren't specific to a single build, but rather touch
#   on all of them.
# ------------------------------------------------------------------------------

GLOBAL_VAR_LIST_GENERAL := CXX FLAGS SOURCE BUILD MODULES	\
  HEADER_EXTENSION APP_EXTENSION OBJECT_EXTENSION					\
  EXECUTABLE_EXTENSION

# List of all global variables that are expected to be customized according to
#   the use of the user.
GLOBAL_VAR_LIST := $(GLOBAL_VAR_LIST_MAIN) $(GLOBAL_VAR_LIST_UNIT) $(GLOBAL_VAR_LIST_GENERAL)

DEFAULT_CXX   := g++
DEFAULT_FLAGS := -std=c++11 -Wall

DEFAULT_SOURCE := src

DEFAULT_BUILD := build

DEFAULT_MODULES :=

DEFAULT_HEADER_EXTENSION     := hpp
DEFAULT_APP_EXTENSION        := cpp
DEFAULT_OBJECT_EXTENSION     := o
DEFAULT_EXECUTABLE_EXTENSION := exe

# An empty space character
define C_SP =
 
endef

# A newline character
define C_NL :=


endef
