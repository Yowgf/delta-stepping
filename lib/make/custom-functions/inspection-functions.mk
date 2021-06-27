# delta-stepping project Makefile
# Custom INSPECTION functions
# Prefix i_
#
# Functions to automatically perform certain checks commonly
#   needed throughout the code.
# ==============================================================

# Category: Variable definition inspection
# Description: Inspects if every variable has been defined.
# Parameter: List with variable names
# Returns: Undefined variable names and status (number of
#          undefined variables found). If no variable is found to
#          be undefined, then it simply returns "0".
define i_check_vars =
	let "i = 1";
	let "status = 0";
	declare -a undefined_vars;
	
	for var in $(foreach provided_var, $(1), "$($(provided_var))"); do
		if [[ "$$var" = "" ]]; then
			(( undefined_vars[$$status] = ($$i-1) ));
			(( status += 1 ));
		fi;
		(( i+=1 )) ;
	done;
	
	declare -a temp_provided_vars=($(1));
	for (( i=0; (( $$i < $$status )); (( i+=1 )) )); do
		undefined_vars[$$i]=$${temp_provided_vars[$${undefined_vars[$$i]}]};
	done;
	
	echo "$${undefined_vars[@]}";
	echo "$$status";
	exit "$$status";
endef
