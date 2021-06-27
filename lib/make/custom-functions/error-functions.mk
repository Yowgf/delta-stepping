# delta-stepping project Makefile
# Custom ERROR functions.
# ==============================================================

# Category: Fatal error.
# Reason: Variable left undefined.
# Description: Prints error message according to parameter, and exits.
# Parameter: Undefined variable name.
define e_undef_var =
  $(error Variable(s) "$(1)" left undefined, please check variable definition file)
endef

# Category: Fatal error.
# Description: Generic fatal error, displays informative message.
# Parameter: Message to be displayed.
define e_fatal =
	$(error $(1));
endef

# Category: Shell fatal error.
# Description: Emits informative error message about last command,
#              exiting thereafter the presently open shell.
# Parameter: Message indicating context.
# Return: Parameter message, followed by status informative message.
define e_prev_cmd =
	{ let "status = $$?"; \
			echo $(1); \
			echo Shell exit status: $$status; \
			exit $$status; \
	}
endef
