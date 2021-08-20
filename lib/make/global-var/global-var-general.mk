# delta-stepping project Makefile
# General global variables custom definitions
# ==============================================================================
# Things included here are aren't specific to a single build, but rather touch
#   on all of them.
# ------------------------------------------------------------------------------

# Compiler command and flags
CXX   := 
FLAGS := -std=c++14 -g -Wextra -fopenmp

# Source directory
SOURCE := lib

# Directory of compiled files
BUILD :=

# Project's modules
MODULES := Alg DS Interface Utils

# Files extensions
HEADER_EXTENSION     := 
APP_EXTENSION        := 
OBJECT_EXTENSION     := 
EXECUTABLE_EXTENSION := 
