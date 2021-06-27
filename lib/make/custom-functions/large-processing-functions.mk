# delta-stepping project makefile
# Custom LARGE PROCESSING functions
# Prefix lp_
#
# Handy general purpose functions that avoid repetitive code, by
#   taking a batch of arguments and applying a certain procedure
#   into it, as opposed to inserting the same code every time
#   that behaviour is needed.
# ==============================================================

# Category: Variable handling
# Description: Applies default value to variables. By default
#              value, let it be understood DEFAULT_<VAR_NAME>.
# Parameter: A list containing the name of the variables
# Returns: Nothing
define newline


endef
define lp_apply_defaults =
$(eval $(foreach var, $(1), ifndef $(var)$(newline)$(var) = $(value DEFAULT_$(var))$(newline)endif$(newline)))
endef
